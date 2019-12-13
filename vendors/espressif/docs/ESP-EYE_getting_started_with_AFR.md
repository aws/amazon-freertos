# Getting Started with the Espressif ESP-EYE

This tutorial provides instructions for getting started with the Espressif  ESP-EYE . To purchase one from our partner on the AWS Partner Device catalog, use the following links: [ESP-EYE]().
ESP-EYE supports on Amazon FreeRTOS. For more information about board, see [ESP-EYE](https://github.com/espressif/esp-who/blob/master/docs/en/get-started/ESP-EYE_Getting_Started_Guide.md) on the github.

**Note**

Currently, the Amazon FreeRTOS port for ESP32-WROVER-KIT and ESP DevKitC does not support the following features:

-   Symmetric multiprocessing (SMP).


## Overview

This tutorial contains instructions for the following getting started steps:

1.  Connecting your board to a host machine.

2.  Installing software on the host machine for developing and debugging embedded applications for your microcontroller board.

3.  Cross compiling an Amazon FreeRTOS demo application to a binary image.

4.  Loading the application binary image to your board, and then running the application.

5.  Interacting with the application running on your board across a serial connection, for monitoring and debugging purposes.


## Prerequisites

Before you get started with Amazon FreeRTOS on your Espressif board, you need to set up your AWS account and permissions.

To create an AWS account, see [Create and Activate an AWS Account](https://aws.amazon.com/premiumsupport/knowledge-center/create-and-activate-aws-account/).

To add an IAM user to your AWS account, see [IAM User Guide](https://docs.aws.amazon.com/IAM/latest/UserGuide/). To grant your IAM user account access to AWS IoT and Amazon FreeRTOS, attach the following IAM policies to your IAM user account:

-   `AmazonFreeRTOSFullAccess`

-   `AWSIoTFullAccess`


**To attach the AmazonFreeRTOSFullAccess policy to your IAM user**

1.  Browse to the [IAM console](https://console.aws.amazon.com/iam/home), and from the navigation pane, choose **Users**.

2.  Enter your user name in the search text box, and then choose it from the list.

3.  Choose **Add permissions**.

4.  Choose **Attach existing policies directly**.

5.  In the search box, enter **`AmazonFreeRTOSFullAccess`**, choose it from the list, and then choose **Next: Review**.

6.  Choose **Add permissions**.


**To attach the AWSIoTFullAccess policy to your IAM user**

1.  Browse to the [IAM console](https://console.aws.amazon.com/iam/home), and from the navigation pane, choose **Users**.

2.  Enter your user name in the search text box, and then choose it from the list.

3.  Choose **Add permissions**.

4.  Choose **Attach existing policies directly**.

5.  In the search box, enter **`AWSIoTFullAccess`**, choose it from the list, and then choose **Next: Review**.

6.  Choose **Add permissions**.


For more information about IAM and user accounts, see [IAM User Guide](https://docs.aws.amazon.com/IAM/latest/UserGuide/).

For more information about policies, see [IAM Permissions and Policies](https://docs.aws.amazon.com/IAM/latest/UserGuide/introduction_access-management.html).

## Set Up the Espressif Hardware

See the [ESP-EYE](https://github.com/espressif/esp-who/blob/master/docs/en/get-started/ESP-EYE_Getting_Started_Guide.md) for information about setting up the ESP-EYE development board hardware.


## Set Up Your Development Environment

To communicate with your board, you need to download and install a toolchain.

### Setting Up the Toolchain

To set up the toolchain, follow the instructions for your host machine's operating system:

-   [Standard Setup of Toolchain for Windows](https://docs.espressif.com/projects/esp-idf/en/v3.3/get-started-cmake/windows-setup.html)

-   [Standard Setup of Toolchain for macOS](https://docs.espressif.com/projects/esp-idf/en/v3.3/get-started-cmake/macos-setup.html)

-   [Standard Setup of Toolchain for Linux](https://docs.espressif.com/projects/esp-idf/en/v3.3/get-started-cmake/linux-setup.html)


**Note**

Version 3.3 of the ESP-IDF (the version used by Amazon FreeRTOS) does not support the latest version of the ESP32 compiler. You must use the compiler that is compatible with version 3.3 of the ESP-IDF (see the links above). To check the version of your compiler, run "xtensa-esp32-elf-gcc --version".

### Install CMake

The CMake build system is required to build the Amazon FreeRTOS demo and test applications for this device. Amazon FreeRTOS supports versions 3.13 and later.

You can download the latest version of CMake from [CMake.org](https://cmake.org/download/). Both source and binary distributions are available.

For more details about using CMake with Amazon FreeRTOS, see [Using CMake with Amazon FreeRTOS](https://docs.aws.amazon.com/freertos/latest/userguide/getting-started-cmake.html).

## Establish a Serial Connection

To establish a serial connection between your host machine and the ESP-EYE, you must install CP210x USB to UART Bridge VCP drivers. You can download these drivers from [Silicon Labs](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).

For more information, see [Establish Serial Connection with ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/establish-serial-connection.html). After you establish a serial connection, make a note of the serial port for your board's connection. You need it when you build the demo.

## Download and Configure Amazon FreeRTOS

After your environment is set up, you can download Amazon FreeRTOS from [GitHub](https://github.com/aws/amazon-freertos), or from the [Amazon FreeRTOS console](https://console.aws.amazon.com/freertos). See the [README.md](https://github.com/aws/amazon-freertos/blob/master/README.md) file for instructions.

### Configure the Amazon FreeRTOS Demo Applications

1.  If you are running macOS or Linux, open a terminal prompt. If you are running Windows, open **mingw32.exe**.

2.  To verify that you have Python 2.7.10 or later installed, run **python --version**. The version installed is displayed. If you do not have Python 2.7.10 or later installed, you can install it from the [Python website](https://www.python.org/downloads/).

3.  You need the AWS CLI to run AWS IoT commands. If you are running Windows, use the **easy_install awscli** to install the AWS CLI in the mingw32 environment.

    If you are running macOS or Linux, see [Installing the AWS Command Line Interface](https://docs.aws.amazon.com/cli/latest/userguide/installing.html).

4.  Run **aws configure** and configure the AWS CLI with your AWS access key ID, secret access key, and default region name. For more information, see [Configuring the AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-getting-started.html).

5.  Use the following command to install the AWS SDK for Python (boto3):

    -   On Windows, in the mingw32 environment, run **easy_install boto3**.

    -   On macOS or Linux, run **pip install tornado nose --user** and then run **pip install boto3 --user**.



Amazon FreeRTOS includes the `SetupAWS.py` script to make it easier to set up your Espressif board to connect to AWS IoT. To configure the the script, open _``<amazon-freertos>``_``/tools/aws_config_quick_start/configure.json`` and set the following attributes:

`afr_source_dir`

The complete path to the _`<amazon-freertos>`_ directory on your computer. Make sure that you use forward slashes to specify this path.

`thing_name`

The name that you want to assign to the AWS IoT thing that represents your board.

`wifi_ssid`

The SSID of your Wi-Fi network.

`wifi_password`

The password for your Wi-Fi network.

`wifi_security`

The security type for your Wi-Fi network.

Valid security types are:

-   `eWiFiSecurityOpen` (Open, no security)

-   `eWiFiSecurityWEP` (WEP security)

-   `eWiFiSecurityWPA` (WPA security)

-   `eWiFiSecurityWPA2` (WPA2 security)


**To run the configuration script**

1.  If you are running macOS or Linux, open a terminal prompt. If you are running Windows, open mingw32.exe.

2.  Go to the _`<amazon-freertos>`_``/tools/aws_config_quick_start`` directory and run ``python SetupAWS.py setup``.


The script does the following:

-   Creates an IoT thing, certificate, and policy

-   Attaches the IoT policy to the certificate and the certificate to the AWS IoT thing

-   Populates the `aws_clientcredential.h` file with your AWS IoT endpoint, Wi-Fi SSID, and credentials

-   Formats your certificate and private key and writes them to the `aws_clientcredential.h` header file


For more information about `SetupAWS.py`, see the ` README.md` in the _`<amazon-freertos>`_`/tools/aws_config_quick_start` directory.

## Build, Flash, and Run the Amazon FreeRTOS Demo Project

You can use CMake to generate the build files, Make to build the application binary, and Espressif's IDF utility to flash your board.

### Build Amazon FreeRTOS

(If you are using Windows, please see the next section.)

Use CMake to generate the build files, and then use Make to build the application.

**To generate the demo application's build files with CMake**

1.  Change directories to the root of your Amazon FreeRTOS download directory.

2.  Use the following command to generate the build files:

```
    cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S . -B <your-build-directory>
```

   **Note**

   If you want to build the application for debugging, add the `-DCMAKE_BUILD_TYPE=Debug` flag to this command.

   If you want to generate the test application build files, add the `-DAFR_ENABLE_TESTS=1` flag.

   If you want to add Lightweight IP (LwIP) support, add the `-DAFR_ESP_LWIP=1` flag.


**To build the application with Make**

1.  Change directories to the `build` directory.

2.  Use the following command to build the application with Make:
```
make all -j4
```

   **Note**

 You must generate the build files with the **cmake** command every time you switch between the `aws_demos` project and the `aws_tests` project.


### Build Amazon FreeRTOS On Windows

On Windows, you must specify a build generator for CMake, otherwise CMake defaults to Visual Studio. Espressif officially recommends the Ninja build system because it works on Windows, Linux and MacOS. You must run CMake commands in a native Windows environment like cmd or powershell. Running CMake commands in a virtual Linux environment, like MSYS2 or WSL, is not supported.

Use CMake to generate the build files, and then use Make to build the application.

**To generate the demo application's build files with CMake**

1.  Change directories to the root of your Amazon FreeRTOS download directory.

2.  Use the following command to generate the build files:

```
    cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -GNinja -S . -B <your-build-directory>
```

   **Note**

   If you want to build the application for debugging, add the `-DCMAKE_BUILD_TYPE=Debug` flag to this command.

   If you want to generate the test application build files, add the `-DAFR_ENABLE_TESTS=1` flag.


**To build the application**

1.  Change directories to the `build` directory.

2.  Invoke Ninja to build the application:

    `ninja`

    Or, use the generic CMake interface to build the application:

    ``cmake --build <your-build-directory>``

**Note**

You must generate the build files with the **cmake** command every time you switch between the `aws_demos` project and the `aws_tests` project.


### Flash and Run Amazon FreeRTOS

Use Espressif's IDF utility (``<amazon-freertos>/vendors/espressif/esp-idf/tools/idf.py``) to flash your board, run the application, and see logs.

To erase the board's flash, go to the _`<amazon-freertos>`_ directory and use the following command:

`./vendors/espressif/esp-idf/tools/idf.py erase_flash -B build`

To flash the application binary to your board, use `make`:

`make flash`

You can also use the IDF script to flash your board:

`./vendors/espressif/esp-idf/tools/idf.py flash -B build`

To monitor:

`./vendors/espressif/esp-idf/tools/idf.py monitor -p /dev/ttyUSB1 -B build`

**Note**

You can combine these commands. For example:

`./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -p /dev/ttyUSB1 -B build`

### Monitoring MQTT Messages on the Cloud

You can use the MQTT client in the AWS IoT console to monitor the messages that your device sends to the AWS Cloud.

**To subscribe to the MQTT topic with the AWS IoT MQTT client**

1.  Sign in to the [AWS IoT console](https://console.aws.amazon.com/iotv2/).

2.  In the navigation pane, choose **Test** to open the MQTT client.

3.  In **Subscription topic**, enter **`iotdemo/#`**, and then choose **Subscribe to topic**.

### Run the Bluetooth Low-Energy Demos

Amazon FreeRTOS supports [Bluetooth Low Energy](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-ble-library.html) connectivity.

To run the Amazon FreeRTOS demo project across Bluetooth Low Energy, you need to run the Amazon FreeRTOS Bluetooth Low Energy Mobile SDK Demo Application on an iOS or Android mobile device.

**To set up the the Amazon FreeRTOS Bluetooth Low Energy Mobile SDK demo application**

1.  Follow the instructions in [Mobile SDKs for Amazon FreeRTOS Bluetooth Devices](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-ble-mobile.html) to download and install the SDK for your mobile platform on your host computer.

2.  Follow the instructions in [Amazon FreeRTOS Bluetooth Low Energy Mobile SDK Demo Application](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-sdk-app) to set up the demo mobile application on your mobile device.


For instructions about how to run the MQTT over Bluetooth Low Energy demo on your board, see the [MQTT over Bluetooth Low Energy Demo Application](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-mqtt).

For instructions about how to run the Wi-Fi provisioning demo on your board, see the [Wi-Fi Provisioning Demo Application](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-wifi).

## Troubleshooting

-   The monitor utility provided by ESP-IDF (and invoked using **make monitor**) helps you decode addresses. For this reason, it can help you get some meaningful backtraces in the event the application crashes. For more information, see [Automatically Decoding Addresses](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html#automatically-decoding-addresses) on the Espressif website.

-   It is also possible to enable GDBstub for communication with gdb without requiring any special JTAG hardware. For more information, see [Launch GDB for GDBStub](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html#launch-gdb-for-gdbstub) on the Espressif website.

-   If `pyserial` cannot be installed using `pip` on macOS, download it from the [pyserial website](https://pypi.org/simple/pyserial).

-   If the board resets continuously, try erasing the flash by entering the following command on the terminal:

    `make erase_flash`

-   If you see errors when you run `idf_monitor.py`, use Python 2.7.

-   Required libraries from ESP-IDF are included in Amazon FreeRTOS, so there is no need to download them externally. If the `IDF_PATH` environment variable is set, we recommend that you clear it before you build Amazon FreeRTOS.

-   On Windows, it can take 3-4 minutes for the project to build. You can use the `-j4` switch on the **make** command to reduce the build time:

    `make flash monitor -j4`

-   If your device has trouble connecting to AWS IoT, open the `aws_clientcredential.h` file, and verify that the configuration variables are properly defined in the file. `clientcredentialMQTT_BROKER_ENDPOINT[]` should look like ``<1234567890123>-ats.iot.<us-east-1>.amazonaws.com``.
- **JTAG Debugging is not supported on ESP-EYE**


For general troubleshooting information about Getting Started with Amazon FreeRTOS, see [Troubleshooting Getting Started](https://docs.aws.amazon.com/freertos/latest/userguide/gsg-troubleshooting.html).
