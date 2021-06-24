The following demos shows how to connect BLE devices to AWS IoT using a smartphone application as the companion device. Smartphone applications are written using FreeRTOS mobile device SDKS for [IOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) and [Android](https://github.com/aws/amazon-freertos-ble-android-sdk). As a pre-requisite to running these demos, the smartphone application has to create an authenticated, secure connection to AWS IoT using Amazon Cognito service. To get started on creating a mobile application, check the samples provided within the respective repositories. 

## BLE GATT server demo
This demo shows how to get started writing a GATT service using BLE APIs on FreeRTOS. The demo creates a GATT service which simply increments a counter value and sends this as a notification over a GATT characteristic. There are also other GATT characteristics which allow reading counter values, stopping or resetting the counter value form a mobile application.

The corresponding mobile devie application runs in GATT client mode, discovers and connects to this GATT service. It reads the counter value and publish them to the AWS IoT on a user configured topic within the mobile application.

The iOS and Android application runs this demo concurrently with other demos by creating separate connections to AWS IoT. 

## MQTT BLE Transport Demo
This demo shows the use of BLE transport interface to send MQTT packets to the cloud using a mobile device application. The mobile application runs in background in passthrough mode to transparently forward packets to AWS IoT. The demo facilitates applications written using coreMQTT library to seamlessly send to and receive data from the cloud over BLE. 

## Shadow BLE Transport Demo
This demo shows how to perform shadow updates for a BLE device with AWS IoT using a companion mobile device application. The demo transfers shadow messages using coreMQTT library and underlying BLE transport interface to seamlessly perform shadow updates with the cloud over BLE. 

## OTA BLE Transport Demo
This demo shows Over The Air update procedure of BLE firmware using a companion mobile device application. The demo uses OTA library to receive and process firmware updates from AWS IoT. The firmware updates are transferred using MQTT packets over BLE transport interface assisted by the companion mobile application.
