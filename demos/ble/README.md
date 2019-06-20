## BLE GATT server demo
This demo demonstrate the Amazon FreeRTOS GATT server by sending a simple counter value to a mobile device application coded against the [iOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) and [Android](https://github.com/aws/amazon-freertos-ble-android-sdk) device SDK for BLE and Amazon FreeRTOS.
The libraries in the iOS and Android SDK will seamlessly publish the messages to the cloud through MQTT.
The mobile application demo can be found in the same [iOS](https://github.com/aws/amazon-freertos-ble-ios-sdk) and [Android](https://github.com/aws/amazon-freertos-ble-android-sdk) SDK. 

In addition this demo show that users can create their own GATT client (on the mobile device) to connect to the GATT server (on the device) in parallel to the iOS or Android SDK GATT client used for MQTT and WiFi provisioning.
It is possible to have the iOS or Android SDK perform the MQTT demo, while interacting at the same time with a custome service on the device.

The iOS and Android SDK can run this demo at the same time as MQTT demo by creating two GATT servers to demonstrate this ability. 
This demo creates a special service with two characteristics, one used to count, the other used to control the counter. From the application, the user can chose to stop/start/reset the counter.
