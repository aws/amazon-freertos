# Amazon FreeRTOS port for SoftHSMv2 #
## Introduction ##
This directory includes a C module that allows [SoftHSMv2](https://github.com/opendnssec/SoftHSMv2), a 3rd party open-source PKCS #11 library, to be used with the [Amazon FreeRTOS simulator project](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html). This port can be used as a rough measurement of PKCS #11 standard compatibility for crypto-aware apps and other PKCS #11 interface implementations. Instructions follow.

## Baseline your configuration ##

Start by following the [Getting Started instructions for the Amazon FreeRTOS simulator](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html). Complete your AWS IoT Thing (i.e. device) configuration and ensure that you can successfully run the [MQTT Echo demo](https://github.com/aws/amazon-freertos/blob/master/demos/mqtt/iot_demo_mqtt.c) and [PKCS #11 tests for Amazon FreeRTOS](https://github.com/aws/amazon-freertos/blob/master/libraries/abstractions/pkcs11/test/iot_test_pkcs11.c). 

## Install SoftHSM ##
Install the latest SoftHSM package for Windows. The port has previously been tested with [this installer](https://github.com/disig/SoftHSM2-for-Windows/releases/download/v2.5.0/SoftHSM2-2.5.0.msi).

## Configure SoftHSM ##
Launch a new cmd.exe window. Initialize the module:
```
c:\SoftHSM2\bin\softhsm2-util.exe --init-token --slot 0 --label "My token 1"
```
When prompted, enter the security officer (SO) and user PIN as 0000. This is appropriate for lab testing purposes only.

## Project integration ##
Open the [Amazon FreeRTOS test project for Visual Studio](https://github.com/aws/amazon-freertos/blob/master/projects/pc/windows/visual_studio/aws_tests/aws_tests.sln). 
Enable full PKCS #11 tests in $\vendors\pc\boards\windows\aws_tests\config_files\aws_test_runner_config.h:
```
#define testrunnerFULL_PKCS11_ENABLED                 1
```
Ensure that your configuration for networking and AWS IoT endpoint and Thing settings are working, as per the Baseline section above.

Modify the Visual Studio solution to include the new file and filter $\vendors\pc\boards\windows\ports\pkcs11\softhsm\iot_pkcs11_softhsm.c. Then remove the filters for abstractions\pkcs11\mbedtls and abstractions\pkcs11\include.