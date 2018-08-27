# Change Log for Amazon FreeRTOS

## V1.4.0 08/27/2018
### New Features

#### Demo Bootloader for Microchip Curiosity PIC32MZEF V1.0.0
The demo bootloader supports Amazon FreeRTOS over-the-air (OTA) update by implementing firmware version checking, cryptographic signature verification, and application self-test. The firmware verification includes verifying the authenticity and integrity of the new firmware received over the air. The bootloader verifies the cryptographic signature of the application before boot. The elliptic-curve digital signature algorithm (ECDSA) with SHA256 is used. The utilities provided can be used to generate a signed application that can be flashed on the device. This enables signature verification of the initial image.

### Updates

#### Over-the-air (OTA) updates
The over-the-air (OTA) updates feature of Amazon FreeRTOS is now generally available. The release includes enhancements to the OTA Agent and changes to the OTA Portable Abstraction Layer (PAL) interface.

#### OTA PAL for Curiosity PIC32MZEF V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for Windows Simulator V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for CC3220SF-LAUNCHXL V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for Espressif ESP32 V1.0.0
Update for API changes for OTA general availability release.

#### OTA Agent V1.0.0
Enhancements and API changes for OTA general availability release.


## V1.3.2 08/21/2018
### New Features
None

### Updates

#### FreeRTOS+TCP 
- Multiple security improvements and fixes in packet parsing routines, DNS caching, and TCP sequence number and ID generation.
- Disable NBNS and LLMNR by default.
- Add TCP hang protection by default.

#### Secure Sockets for FreeRTOS+TCP 
- Improve security in Amazon FreeRTOS Secure Sockets usage of mbedTLS and ALPN.

We thank Ori Karliner of Zimperium zLabs Team for reporting these issues.


## V1.3.1 08/10/2018
### New Features
None

### Updates

#### OTA Beta Update
- Updates to OTA Beta to incorporate a new API for the OTA service. This API is not compatible with the  API used in the original OTA Beta released on Dec. 20th. 
- Add a "reference bootloader" for use in the OTA process. This bootloader is for use with the Microchip MCU.

#### Amazon FreeRTOS Qualification Program (AFQP) Update
- Update AFQP documentation. For more info on the changes to AFQP, reference the Revision History of the "Amazon FreeRTOS Qualification Program Developer Guide" in the "tests" directory.

#### Device Defender Update
- Add a demo to illustrate the operation of Device Defender for the Windows Simulator and Microchip PIC32MZEF MCU.

#### TI "Hello World" Build Failure Resolution
- Address an issue where the "Hello World" demo did not build with TI CCS 7.3 in AFR 1.3.0 when downloaded from OCW.


## V1.3.0 07/31/2018

### New Features

#### AFQP 1.0 Support
This release of AFR has support for vendors who wish to have their ports qualified for Amazon FreeRTOS through the Amazon FreeRTOS Qualification Program (AFQP). This is the first public release of AFQP tests and documentation. A new top level "tests" directory is added to support this functionality. AFQP documents are available in "tests" directory. [Learn more.](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-qualification-program.html)

#### Device Defender 1.0 Support
AWS IoT Device Defender is an AWS IoT security service that allows users to audit the configuration of their devices, monitor connected devices to detect abnormal behavior, and to mitigate security risks. It gives you the ability to enforce consistent security policies across your AWS IoT device fleet and respond quickly when devices are compromised. Device side support of this feature is part of this release. Devices supported are WinSim and Microchip PIC32MZEF. [Learn more.](https://docs.aws.amazon.com/freertos/latest/userguide/afr-device-defender-library.html)

#### FreeRTOS+POSIX 1.0 Interface
This release includes version 1.0.0 of FreeRTOS+POSIX.  FreeRTOS+POSIX is a POSIX compatibility layer that allows existing POSIX applications to run without modifications of FreeRTOS.  This release supports POSIX threads, mutexes, barriers, condition variables, semaphores, message queues, clocks, timers, and error numbers.  While most of the POSIX functions are implemented and up to specification, limitations in the FreeRTOS kernel precluded the standard implementations of certain functions.  The functions which differ from the POSIX specification are identified in their header files. Currently, FreeRTOS+POSIX is only used by drivers of the TI CC3220SF.

### Updates

#### FreeRTOS Kernel V10.0.1
- Add Idle tick counter interface
- Rename posix/ to FreeRTOS_POSIX/

#### FreeRTOS+TCP V2.0.4
- Fix issues raised by the Coverity scan
    - Fix a typo ulRxWindowLength -> ulTxWindowLength in FreeRTOS_Sockets.c
    - Fix strncmp length in FreeRTOS_DNS.c
    - Fix styling in FreeRTOS_ARP.c
- Fix a spelling typo ";east" -> "least" (response from a pull request)
- Add auto check of network interfaces for WinSim

#### MQTT Agent V1.1.2
- Move MQTT metrics to agent

#### mbedTLS-based PKCS#11 V1.0.3
- Reduce the number of warnings generated

#### PKCS#11 for LPC54018 IoT Module V1.0.1
- Change project baudrate setting to resolve AFQP test failures

#### Secure Sockets for NXP54018_IoT_Module V1.0.0 Beta 3
- Update to latest NXP driver to address AFQP 1.0 test failures

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 2
- Update to new Inventek driver to resolve AFQP 1.0 test failures

#### Wi-Fi for Curiosity PIC32MZEF V1.0.3
- Change Microchip network param to use a direct address instead of a section to reduce the size of the binary image to allow OTA to continue working.
- Reduce number of warnings generated.

#### Wi-Fi for LPC54018 IoT Module V1.0.1
- Add fixes for Demo and DHCP.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
- Update for release of AFQP 1.0

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.2
- Update for release of AFQP 1.0

---------------------------------------------------------------------------------------

## V1.2.7 05/31/2018

- Update the Texas Instruments SimpleLink CC3220 SDK from version 1.40.01.00 to version 2.10.00.04.
- Fix the MQTT Echo Demo (Hello World Demo) to avoid truncating received strings.
- Modify the Getting Started scripts to check if the AWS CLI is configured.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.4
- Update comments for SimpleLink CC3220 SDK version 2.10.00.04.

--------------

## V1.2.6 05/18/2018

- Fix NXP MCUXpresso project build failure on Linux.

----------------

## V1.2.5 05/14/2018

- Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

#### FreeRTOS+TCP V2.0.4
 - Add Espressif ESP32 network interface support.

#### mbedTLS-based PKCS#11 V1.0.3
 - Implement C_DigestInit, C_DigestUpdate, and C_DigestFinal for SHA-256.
 - Implement C_GenerateKeyPair for non-persistent ECDSA P256.

#### PKCS#11 for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
 - Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
 - Bug fix to ensure that WIFI_ConnectAP() switches to the network parameters input,
      even when already connected to a different set.

#### Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
 - Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

---------------------------------------------------------------------------------------

## V1.2.4 05/01/2018

 - Upgrade to mbedTLS version 2.8.
 - Add MCUXpresso IDE demo project for the NXP LPC54018 IoT Module.

#### Crypto V1.0.2
 - Minor updates due to mbedTLS crypto interface changes.

#### FreeRTOS+TCP V2.0.3
 - Fix a bug where re-transmission and duplicated TCP packets would create a
      computation underflow as well as a memory leak.
 - Add new public function FreeRTOS_UpdateMACAddress() to allow changing the MAC address
      after FreeRTOS_IPInit. Sometimes the device MAC address is not available at the
      time FreeRTOS_IPInit() is called, so it needs to be changed afterward.
 - Remove non-cryptographic rand() implementation.
 - Remove a static variable in functions prvGetHostByName() and prvCreateDNSSocket() to
      make them threadsafe.

#### Greengrass Discovery V1.0.3
 - Provide a helpful error message if the Greengrass Discovery JSON does not fit in
      the supplied buffer.

#### MQTT Agent V1.1.2
 - Bug fix to avoid socket leak if MQTT Connect fails after a successful TCP
      connection.
 - Add support for disabling subscription management feature by defining the macro
      mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT as 0.

#### OTA PAL for Curiosity PIC32MZEF V0.9.1
 - Update for PKCS#11 PAL layer API changes.

#### OTA PAL for Windows Simulator V0.9.2
- Minor restructuring of file locations.

#### OTA PAL for CC3220SF-LAUNCHXL V0.9.3
 - Minor changes to enable test integration.

#### OTA Agent V0.9.4
 - Minor restructuring of file locations.

#### mbedTLS-based PKCS#11 V1.0.2
 - Combine the mbedTLS based PKCS#11 implementation from Curiosity PIC32MZEF,
      LPC54018 IoT Module, Windows Simulator, and STM32L4 Discovery kit IoT node into a
      single file.
 - Add support for public key verification of signatures.
 - Fix to free context structures on session failure.
 - Update C_OpenSession to use CKF_SERIAL_SESSION flag.

#### PKCS#11 for Curiosity PIC32MZEF V1.0.2
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS#11 for LPC54018 IoT Module V1.0.1
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS#11 PAL for Windows Simulator V1.0.2
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS#11 for STM32L4 Discovery kit IoT node V1.0.1
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS#11 for CC3220SF-LAUNCHXL V1.0.2
 - PKCS#11 implementation for TI based on mbedTLS moved into this file.

#### Secure Socket for FreeRTOS+TCP V1.1.2
 - Combine Secure Sockets implementation for Curiosity PIC32MZEF and Windows
      Simulator into a single file.
 - Fix return value of SOCKETS_Socket on error.
 - Attempting to create an unsupported UDP socket now triggers an assert.
 - Add cryptographic random number generator function for TCP sequence numbers.
 - Update the Socket structure to keep track of a connection attempt and added
      support of the ECONN error.

#### Secure Sockets for LPC54018 IoT Module V1.0.0 Beta 3
 - Fix minor bug in SOCKETS_Recv().

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 2
 - Fix return value of SOCKETS_Close on error.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.3
 - Secure sockets printing is now controlled independently using the SOCKETS_PRINT
      macro. SOCKETS_PRINT prints TI driver error codes.

#### Shadow V1.0.3
 - Change names of configuration constants to be consistent with FreeRTOS style.

#### TLS V1.1.1
 - Support AWS IoT Just-in-Time Registration (JITR) by sending optional
      client-issuer certificate.
 - Use CKF_SERIAL_SESSION flag with PKCS#11.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.3
 - Update for setting the MAC Address in WIFI_On() by using new FreeRTOS+TCP
      function FreeRTOS_UpdateMACAddress().
 - Redefine printing and assert stubs used in the Wi-Fi driver code.
 - Add implementation of WIFI_GetMAC().
 - Add implementation of WIFI_IsConnected().
 - Minor bug fixes.

#### Wi-Fi for LPC54018 IoT Module V1.0.2
 - Add implementation of WIFI_IsConnected().
 - Fix max DNS name length redefinition.
 - Fix compiler errors in MCUXpresso IDE.
 - Minor bug fixes.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.1
 - Add implementation of WIFI_IsConnected().
 - Add NULL pointer checks throughout.
 - Minor bug fixes.

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.2
 - Add implementation of WIFI_IsConnected().
 - Add NULL pointer checks throughout.
 - Minor bug fixes.

---------------------------------------------------------------------------------------

## V1.2.3 03/29/2018
 - Fix TI IAR project build failure.

---------------------------------------------------------------------------------------

## V1.2.2 02/27/2018

#### OTA Agent V0.9.3
 - Formatting update.

#### OTA PAL for Curiosity PIC32MZEF V0.9.0
 - Beta release of the OTA Update support for the Microchip Curiosity PIC32MZEF.

#### PKCS#11 for Curiosity_PIC32MZEF V1.0.1
 - Add support for the management of OTA update code signing keys.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.1
 - Update to conditionally compile the entire file.

---------------------------------------------------------------------------------------

## V1.2.1 02/23/2018

 - Add an IAR IDE demo project for the Texas Instruments CC3220SF-LAUNCHXL.
 - Add Wi-Fi support for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS+TCP V2.0.2
 - Improve NULL variable check in buffer management.

#### MQTT Agent V1.1.1
 - Minor bug fix checking for a NULL pointer.

#### OTA Agent V0.9.2
 - Update to support NULL OTA file handles.

#### Amazon FreeRTOS OTA PAL for CC3220SF-LAUNCHXL V0.9.2
 - Update to support NULL OTA file handles.

#### PKCS#11 for CC3220SF-LAUNCHXL V1.0.1
 - Add a dummy variable to a previously empty structure to fix IAR compiler errors.

#### Secure Socket for Windows Simulator V1.1.1
 - Formatting update.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.2
 - Update to print SimpleLink driver-specific error codes when debugging.
 - Add error handling for non-blocking sockets.
 - Update socket option to return an error if security options are specified after
      a connection.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.1
 - Update such that Wi-Fi disconnection waits until the link is down before
      returning.

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.1
 - Fix error in attempting to overwrite a const memory.

---------------------------------------------------------------------------------------

## V1.2.0 02/06/2018

#### Greengrass Discovery V1.0.2
 - Update to send all data until an error is received.

#### MQTT Agent V1.1.0
 - Add support for ALPN. ALPN allows MQTT traffic to be sent to
      the AWS IoT broker over port 443 instead of 8883.

#### OTA Agent V0.9.1
 - Send a FAILED status from agent when a file too large for the platform
      is received.
 - Rename some files.

#### PKCS#11 for Windows Simulator
 - Add developer mode key provisioning support.

#### Secure Socket for Curiosity PIC32MZEF V1.0.1
 - Add support for ALPN.

#### Secure Socket for Windows Simulator V1.1.0
 - Add support for ALPN.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.1
- Remove unnecessary server certificate storage on the client side.
- Remove unnecessary global synchronization semaphore.
- Update for other small bugs.

#### Shadow V1.0.2
- Fix error handling bugs.
- Require client tokens.
- Update for other small bugs.

#### TLS V1.1.0
- Add support for ALPN.

---------------------------------------

## V1.1.0 12/20/2017

#### Crypto V1.0.1
- Fix compiler warning for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS+TCP V2.0.1
- Add support for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS Kernel V10.0.1
- Minor bug fixes to allow Linux and C++ builds.

#### Greengrass Discovery V1.0.1
- Reformat console display messages in order to better facilitate demos and debugging.

#### MQTT Agent V1.0.1
- The MQTT Agent now processes commands between successive socket reads to       enable faster command handling, especially when the connected socket is       receiving data continuously.

#### OTA Agent V0.9.0
- Beta release of OTA Update library for Amazon FreeRTOS. Includes support   for the Texas Instruments CC3220SF-LAUNCHXL and Windows Simulator.

#### PKCS#11 for Curiosity PIC32MZEF V1.0.0 Beta 1
- Add support for the Microchip Curiosity PIC32MZEF.

#### Secure Socket for Curiosity PIC32MZEF V1.0.0
- Add support for the Microchip Curiosity PIC32MZEF.

#### Secure Sockets for LPC54018 IoT Module V1.0.0 Beta 2
- Fix bugs in the Secure Sockets implementation for the NXP LPC54018 IoT       Module.

#### Shadow V1.0.1
- Fix compiler warning for the Microchip Curiosity PIC32MZEF.

#### Wi-Fi for LPC54018 IoT Module V1.0.1
- Change the Wi-Fi Connection timeout to 10 seconds.
