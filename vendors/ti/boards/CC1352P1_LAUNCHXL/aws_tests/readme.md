aws_tests
=========

## Introduction

The aws_tests project is used to dispatch various test cases on the LaunchPad
in order to verify the correctness of TI's port.

The following test suites are currently supported:

 - common-io
 - ble_hal

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
The tested version is 4.30.02.xx where xx is an incrementing build number.

1. Install CC13x2_26x2 SDK and sysconfig from [My Secure Software](https://www.ti.com/securesoftware/docs/autopagepreview.tsp?opnId=25111)
1. Symlink the SDK from its install location (e.g. `C:\ti` or `/home/ti`) to
   `<AFR_REPO>/vendors/ti/simplelink_cc13x2_26x2_sdk/<CC13X2_26X2_SDK_VER>`

   - `<AFR_REPO>` is the location of the amazon-freertos repo
   - `<CC13X2_26X2_SDK_VER>` is the version of the CC13x2_26X2 SDK, underscore
     separated. For example `4.30.02.10` --> `4_30_02_10`
1. Symlink sysconfig from its install location (e.g. `C:\ti` or `/home/ti`) to
   `<AFR_REPO>/vendors/ti/sysconfig/<SYSCONFIG_VER>/`
   - `<SYSCONFIG_VER>` is the version of the Sysconfig tool.
     For example use  `sysconfig_1.6.0` --> `1.6.0`

Note: as an alternative to symlinks, the contents of the sysconfig and
cc13x2_26x2 SDK installers may be copied into their associated location within
the `vendors/ti` folder.

Note 2: Windows users may use powershell to create a symbolic
link (symlink). Please refer to
[this article](https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.management/new-item?view=powershell-7)
for further instructions. The following commands can be used to create the
necessary symlinks for sysconfig and the SDK. Note this assumes powershell as
admin and that the TI content is installed to `C:\ti` which is default.

```
New-Item -ItemType SymbolicLink -Path "<AFR_REPO>\vendors\ti\sysconfig\1.6.0" -Target "C:\ti\sysconfig_1.6.0"
```
and for the SDK
```
New-Item -ItemType SymbolicLink -Path "<AFR_REPO>\vendors\ti\simplelink_cc13x2_26x2_sdk\4_30_02_10" -Target "C:\ti\simplelink_cc13x2_26x2_sdk_4_30_02_10_eng"
```

## Building

CMake is a two step process. The first step is generating the build files
This can be done with the following command:

Replace CC1352P1_LAUNCHXL with CC1352R1_LAUNCHXL if you wish to use that board.
Note: if you are on a system that supports unix makefiles, you may switch from
the MinGW Makefiles generator (`-G`). See [CMake Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
for more information.

This script should be run at the root of the amazon-freertos repo.

```
cmake -DVENDOR=ti -DBOARD=CC1352P1_LAUNCHXL -DCOMPILER=arm-gcc -DCMAKE_BUILD_TYPE=debug -S . -B build -G"MinGW Makefiles" -DAFR_ENABLE_DEMOS=0 -DAFR_ENABLE_TESTS=1 -DAFR_ENABLE_ALL_MODULES=0 -DCMAKE_EXPORT_COMPILE_COMMANDS=1  -DCC13X2_26X2_SDK_VER=4_30_02_10 -DSYSCONFIG_VER=1.6.0
```

Note that the variable `CC13X2_26X2_SDK_VER=4_30_02_10` must be set to the same
as created by the symlink in the dependency setup steps. Similiarly,
`SYSCONFIG_VER=1.6.0` must be set to the same as created by the symlink in the
dependency setup steps.

After the configuration is complete, the binary can be built using

```
cmake --build build/
```

The output binary is located in `build/aws_tests.out`. This binary can be loaded
using Uniflash.


## Hardware setup

The aws_test project when testing common-io is setup to run on a LaunchPad using
the following hardware setup:

### GPIO:

DIO07, DIO06 and DIO26 assigned to GPIO index 0, 1 and 2 respectively.

Mount jumper to connect DIO07 and DIO06.

### I2C:

SCL at DIO4 and SDA at DIO5.

The vendor/ti/simplelink_common/common_io/iot_test_common_io_internal.c file
specifies the device address and register to verify functionality against.

### SPI:

No device needed, tests are currently single sided only.

### UART:

Uses UART1 module for testing,

RX at DIO26 and TX at DIO27.

Mount jumper to connect DIO26 and DIO27 to support loopback test.

The AFQP_IotUARTWriteAsyncReadAsyncLoopbackTest test case may fail as we are
not able to capture in flight byte count without making it device specific.
This means it will return 0 until the TX operation is done.

### PWM:

Uses PWM0. PWM IO set to DIO27 to support measuring of frequency via the GPIO
driver. Jumper mounted to connect DIO26 and DIO27.

### ADC:

Uses two channels, 0 and 1. This corresponds to DIO23 and DIO24 where DIO24 need
to be short circuit to 3V3.

The `AFQP_IotAdcIoctlSetConfigFuzzy` will fail due to the "wrong" value being
reported back. This is due to a mismatch between ADC test and ADC common-io
API definition where the driver follows the latter.

### PERFCOUNTER:

The AFQP_IotPerfCounterGetValueWithDelay test may also fail if the device is
allowed to enter standby as the DWT count value seem to reset.
As long as standby is prohibited the test will pass.

## Running the tests

Once the binary is flashed the tests will run by default. Both BLE and common-io
are enabled. The steps below assume that the raspberry pi is setup, which is
described in the next section.

1. Run `<AFR_REPO>/libraries/abstractions/ble_hal/test/ble_test_scipts/runPI.sh`
1. Open [PUTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
   terminal using the following settings:

     - Serial line: COM port of user/UART port of LaunchPad
     - Baudrate : 115200
     - Terminal -> check boxes for "Implicit CR in every LF" and
       "Implicit LF in every CR"

1. Test results from the DUT will be printed in putty, results from the
   raspberry pi are printed in its ssh terminal

### Setting up the raspberry pi

The BLE tests require a connection to the raspberry-pi. Amazon has setup
instructions [here](https://docs.aws.amazon.com/freertos/latest/userguide/afr-bridgekeeper-dt-bt.html#dt-bt-pi-setup).
An AWS account is not required. Instead, do the following:

1. Make a manual keyboard and HDMI connection to the PI
1. Login with user=root password=idtafr
1. Start ethernet with `ifup eth0` (after plugging in ethernet cable)
1. Make note of IP address
1. Update `<AFR_REPO>/libraries/abstractions/ble_hal/test/ble_test_scipts/runPI.sh`
   with the IP address from the previous step.


## Expected Results BLE Port

The BLE HAL port is not yet fully AFQP compliant.
It has support for the following test cases and their associated APIs:

```
    BLE_Initialize_BLE_GATT
    BLE_CreateAttTable_CreateServices
    BLE_CreateAttTable_StartService
    BLE_Initialize_common_GAP
    BLE_Initialize_BLE_GAP
    BLE_Advertising_SetProperties
    BLE_Advertising_SetAvertisementData
    BLE_Advertising_StartAdvertisement
    BLE_Connection_SimpleConnection
    BLE_DeInitialize
    BLE_Free
```

In order to streamline testing to only the supported set of APIs, the tester
can remove known failing tests.

1. Navigate to `libraries/abstractions/ble_hal/test/src/iot_test_ble_hal_afqp.c`
   Comment out the following tests inside `TEST_GROUP_RUNNER( Full_BLE )`

    - `BLE_Connection_RemoveAllBonds`
    - `BLE_Property_WriteLongCharacteristic`
    - `BLE_Property_ReadLongCharacteristic`
    - `BLE_Connection_Mode1Level4`
    - `BLE_Connection_Mode1Level4_Property_WriteDescr`
    - `BLE_Connection_Mode1Level4_Property_WriteChar`
    - `BLE_Connection_Disconnect`
    - `BLE_Connection_BondedReconnectAndPair`
    - `BLE_Connection_Disconnect_From_Peripheral`
    - `BLE_Connection_CheckBonding`
    - `BLE_Connection_RemoveBonding`
    - `BLE_Connection_Mode1Level2`

1. Navigate to `libraries/abstractions/ble_hal/test/ble_test_scipts/startTests_afqp.py`

   Comment out the following tests:

   ```
        isTestSuccessFull = runTest.writereadLongCharacteristic()
        runTest.submitTestResult(
            isTestSuccessFull,
            runTest.writereadLongCharacteristic)
   ```

   and the following

   ```
        isTestSuccessFull = runTest.pairing()
        runTest.submitTestResult(isTestSuccessFull, runTest.pairing)
        bleAdapter.bondToRemoteDevice()

        # Check writing to protected characteristic after successfull pairing
        # succeed
        time.sleep(2)  # wait before starting next test
        isTestSuccessFull = runTest.readWriteProtectedAttributesWhilePaired()
        runTest.submitTestResult(isTestSuccessFull,
                                 runTest.readWriteProtectedAttributesWhilePaired)

        # disconnect, Note it is not a test happening on bluez, the DUT is waiting
        # for a disconnect Cb
        runTest.disconnect()

        # reconnect! Since devices bonded, it should not ask for pairing again.
        # Security agent can be destroyed
        # remove security agent so as not to trigger auto pairing.
        securityAgent.removeSecurityAgent()
        bleAdapter.setDiscoveryFilter(scan_filter)
        # wait for DUT to start advertising
        bleAdapter.startDiscovery(runTest.discoveryStartedCb)
        runTest.mainloop.run()
        bleAdapter.stopDiscovery()
        runTest.reconnectWhileBonded()

        # Test to wait for a disconnect from DUT.
        runTest.waitForDisconnect()

        # reconnect while not bonded. Pairing should fail since Just works is not
        # accepted
        bleAdapter.removeBondedDevices()
        time.sleep(2)  # wait for bonded devices to be deleted
        bleAdapter.setDiscoveryFilter(scan_filter)
        bleAdapter.startDiscovery(runTest.discoveryEventCb)
        runTest.mainloop.run()
        bleAdapter.stopDiscovery()

        agent = securityAgent.createSecurityAgent("NoInputNoOutput", agent)
        runTest.reconnectWhileNotBonded()
   ```


With the above changes in place the expected (ble related) output from the


```

TEST(Full_BLE, BLE_Setup) PASS
TEST(Full_BLE, BLE_Initialize_common_GAP) PASS
TEST(Full_BLE, BLE_Initialize_BLE_GAP) PASS
TEST(Full_BLE, BLE_Initialize_BLE_GATT) PASS
TEST(Full_BLE, BLE_CreateAttTable_CreateServices) PASS
TEST(Full_BLE, BLE_Advertising_SetProperties) PASS
TEST(Full_BLE, BLE_Advertising_SetAvertisementData) PASS
TEST(Full_BLE, BLE_Advertising_StartAdvertisement) PASS
TEST(Full_BLE, BLE_Connection_SimpleConnection) PASS
TEST(Full_BLE, BLE_Property_WriteCharacteristic) PASS
TEST(Full_BLE, BLE_Property_WriteDescriptor) PASS
TEST(Full_BLE, BLE_Property_ReadCharacteristic) PASS
TEST(Full_BLE, BLE_Property_ReadDescriptor) PASS
TEST(Full_BLE, BLE_Property_WriteNoResponse) PASS
TEST(Full_BLE, BLE_Property_Enable_Indication_Notification) PASS
TEST(Full_BLE, BLE_Property_Notification) PASS
TEST(Full_BLE, BLE_Property_Indication) PASS
TEST(Full_BLE, BLE_Property_Disable_Indication_Notification) PASS
TEST(Full_BLE, BLE_DeInitialize) PASS
TEST(Full_BLE, BLE_Free) PASS

```

The output of the raspberry pi is shown below:

```
TEST(Full_BLE, RaspberryPI_advertisement) PASS
TEST(Full_BLE, RaspberryPI_simpleConnection) PASS
TEST(Full_BLE, RaspberryPI_discoverPrimaryServices) PASS
Dscr: 8a7f1168-48af-4efb-83b5-e679f932000b
Dscr: 8a7f1168-48af-4efb-83b5-e679f9320009
Dscr: 8a7f1168-48af-4efb-83b5-e679f932000a
Char: 3113a187-4b9f-4f9a-aa83-c614e11b0001
Service: 8a7f1168-48af-4efb-83b5-e679f9320001
Dscr: 8a7f1168-48af-4efb-83b5-e679f9320008
Dscr: 00002902-0000-1000-8000-00805f9b34fb
Char: 8a7f1168-48af-4efb-83b5-e679f9320007
Service: 3113a187-4b9f-4f9a-aa83-c614e11b0000
Char: 8a7f1168-48af-4efb-83b5-e679f9320006
Dscr: 00002902-0000-1000-8000-00805f9b34fb
Char: 8a7f1168-48af-4efb-83b5-e679f9320004
Char: 8a7f1168-48af-4efb-83b5-e679f9320005
Char: 8a7f1168-48af-4efb-83b5-e679f9320002
Service: 00001801-0000-1000-8000-00805f9b34fb
Char: 8a7f1168-48af-4efb-83b5-e679f9320003
TEST(Full_BLE, RaspberryPI_stopAdvertisement) PASS
Dscr: 8a7f1168-48af-4efb-83b5-e679f932000b
Dscr: 8a7f1168-48af-4efb-83b5-e679f9320009
Dscr: 8a7f1168-48af-4efb-83b5-e679f932000a
Char: 3113a187-4b9f-4f9a-aa83-c614e11b0001
Service: 8a7f1168-48af-4efb-83b5-e679f9320001
Dscr: 8a7f1168-48af-4efb-83b5-e679f9320008
Dscr: 00002902-0000-1000-8000-00805f9b34fb
Char: 8a7f1168-48af-4efb-83b5-e679f9320007
Service: 3113a187-4b9f-4f9a-aa83-c614e11b0000
Char: 8a7f1168-48af-4efb-83b5-e679f9320006
Dscr: 00002902-0000-1000-8000-00805f9b34fb
Char: 8a7f1168-48af-4efb-83b5-e679f9320004
Char: 8a7f1168-48af-4efb-83b5-e679f9320005
Char: 8a7f1168-48af-4efb-83b5-e679f9320002
Service: 00001801-0000-1000-8000-00805f9b34fb
Char: 8a7f1168-48af-4efb-83b5-e679f9320003
TEST(Full_BLE, RaspberryPI_checkUUIDs) PASS
TEST(Full_BLE, RaspberryPI_checkProperties) PASS
TEST(Full_BLE, RaspberryPI_readWriteSimpleConnection) PASS
TEST(Full_BLE, RaspberryPI_writeWithoutResponse) PASS
TEST(Full_BLE, RaspberryPI_notification) PASS
TEST(Full_BLE, RaspberryPI_indication) PASS
TEST(Full_BLE, RaspberryPI_removeNotification) PASS
TEST(Full_BLE, RaspberryPI_removeIndication) PASS
-----------------------
12 Tests 0 Failures 0 Ignored
```
