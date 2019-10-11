# SNTP Demo Beta

### Note

- This is beta version of  SNTP demo and library code used by the demo. The library does not support server authentication, therefore it is strongly recommened not use this code in production. 
- The demo currently only works with Windows Simulato project, other hardware is not yet supported.

## Configuration

- You will need to complete common setup as described here: 
https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started.html

- Windows simulator specific setup as described here:
https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html

**Note** You do not need to setup AWS account for this demo. 

- In order to run demo, you will need to configure at least one NTP server address. You may use many available public NTP servers for testing, but it recommened to host your server for production. The file you will need to modify to add the server addresses is located here: libraries\c_sdk\standard\ntp\include\iot_sntp_config.h

- You will also need to modify aws_demo_config.h  file to define NTP demo. 
Please replace #define CONFIG_MQTT_DEMO_ENABLED with #define CONFIG_NTP_DEMO_ENABLED

## Demo output

Here is am example output fromsuccesful demo run:

- 10 382 [iot_thread] [INFO ][INIT][382] SDK successfully initialized.
- 11 382 [iot_thread] [INFO ][DEMO][382] Successfully - initialized the demo. Network type for the demo: 4
- 12 382 [iot_thread] [INFO ][DEMO][382] SNTP DEMO starts
- 14 511 [NetUdpRecv] [INFO ][NTP][511] SNTP Round Trip Time:100 ms
- 15 511 [NetUdpRecv] [INFO ][NTP][511] NTP time: 10/11/2019 16:46:57.932
- 16 512 [iot_thread] [INFO ][NTP][512] Current UTC time:: 10/11/2019 16:46:57.933
- 17 5512 [iot_thread] [INFO ][NTP][5512] Current UTC time:: 10/11/2019 16:47:02.933

- ...
- 30 60611 [NetUdpRecv] [INFO ][NTP][60611] SNTP Round Trip Time:60 ms
- 31 60618 [NetUdpRecv] [INFO ][NTP][60611] Clock Error per second:-0.900000 seconds
- 32 60618 [NetUdpRecv] [INFO ][NTP][60618] NTP time: 10/11/2019 16:48:51.563
- 33 65513 [iot_thread] [INFO ][NTP][65513] Current UTC time:: 10/11/2019 16:48:59.465
- 34 70513 [iot_thread] [INFO ][NTP][70513] Current UTC time:: 10/11/2019 16:49:09.465


**Demo output notes**
 - *NTP Time* - Time received from server. This log message will be printed everytime when the SNTP client fetches time from the server. Default NTP polling period is 60 seconds set in iot_sntp_config.h. Since Amazon FreeRTOS Windows Simulator does not have accurate time keeping mechanism, the actual polling period may vary from 60 seconds to 120 seconds. 

 - *Current UTC Time* - Time printed by the demo application between NTP server refresh. 

 - *Clock Error per second* - After the second fetch from the server,  NTP library calculates clock error based on the time received and adds it to the internal time maintained by the library.

- After second fetch time printed by the demo will become much more accurate as clock error is used correct the time.

- *SNTP Round Trip Time* - Round trip time in milliseconds to get resonse back from the server.
 