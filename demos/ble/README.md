## BLE GATT server demo
This demo demonstrate the FreeRTOS GATT server by sending a simple counter value to a mobile device application coded against the [iOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) and [Android](https://github.com/aws/amazon-freertos-ble-android-sdk) device SDK for BLE and FreeRTOS.
The libraries in the iOS and Android SDK will seamlessly publish the messages to the cloud through MQTT.
The mobile application demo can be found in the same [iOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) and [Android](https://github.com/aws/amazon-freertos-ble-android-sdk) SDK. 

In addition this demo show that users can create their own GATT client (on the mobile device) to connect to the GATT server (on the device) in parallel to the iOS or Android SDK GATT client used for MQTT and WiFi provisioning.
It is possible to have the iOS or Android SDK perform the MQTT demo, while interacting at the same time with a custome service on the device.

The iOS and Android SDK can run this demo at the same time as MQTT demo by creating two GATT servers to demonstrate this ability. 
This demo creates a special service with two characteristics, one used to count, the other used to control the counter. From the application, the user can chose to stop/start/reset the counter.

## MQTT BLE Transport Demo
This application demonstrates the transport interface implementation with BLE to send MQTT packets to the cloud via a mobile device application. The demo requires a companion mobile application written using the [iOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) or [Android](https://github.com/aws/amazon-freertos-ble-android-sdk) SDK. Sample mobile application can be found in the respective repositories. The demo facilitates applications written using the MQTT library to seamlessly send to and receive data from the cloud, over BLE through a companion device. 

## Shadow BLE Transport Demo
This application demonstrates Device Shadow updates for a BLE device with AWS IoT using a companion mobile device application. The demo requires a smartphone application written using the companion SDK for [iOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) or [Android](https://github.com/aws/amazon-freertos-ble-android-sdk). Sample mobile applications can be found in the respective repositories. The demo facilitates applications written using the shadow and MQTT libraries to seamlessly perform shadow updates with the cloud over BLE through a companion device. 
