## Introduction

FreeRTOS OTA library defines a set of platform abstraction layer(PAL) APIs in `libraries/freertos_plus/aws/ota/src/aws_iot_ota_pal.h`. This implementation of FreeRTOS OTA PAL is based on PSA Functional APIs. It enables FreeRTOS OTA library to perform secure OTA updates on Trusted Firmware-M enabled Cortex-M devices. Trusted Firmware-M provides Secure Processing Environment on Cortex-M devices and implements PSA FUnctional APIs. For more details, please refer to section [What is this project](https://github.com/Linaro/amazon-freertos/blob/tfm-fwu/libraries/abstractions/ota_pal_psa/README.md#what-is-this-project).

## Get the Source Files

The source files of the implementation are [here](https://github.com/Linaro/amazon-freertos/tree/tfm-fwu/libraries/abstractions/ota_pal_psa). Please copy all these source files into this folder for integration.

## Integration Guide

Please refer to section [Integration guide](https://github.com/Linaro/amazon-freertos/tree/tfm-fwu/libraries/abstractions/ota_pal_psa#integration-guide) for how to use this OTA PAL implementation in a project.
