## Script to setup the AWS resources through command line
This script automates the process of [Prerequisites](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-prereqs.html)
and the configuring the files `aws_clientcredential.h` and `aws_cliencredential_keys.h` to connect to AWS IoT.

Make sure you have `aws cli` configured on your machine with access_key, secret_key and region.

Open the file `configure.json` and fill in the following details:
    - thing_name : Name of the thing you want to create
    - wifi_ssid : The SSID of the wifi you want to use.
    - wifi_password : The password of your wifi.
    - wifi_security : The security type for your WiFi network. Valid security types are:
        - eWiFiSecurityOpen: Open, no security
        - eWiFiSecurityWEP: WEP security
        - eWiFiSecurityWPA: WPA security
        - eWiFiSecurityWPA2: WPA2 security

**Options to use with the script**

1. To setup your Thing, type the command: `python SetupAWS.py setup`
2. To cleanup the Thing you created with the script, type the command: `python SetupAWS.py cleanup`
3. To list your certificates, type the command: `python SetupAWS.py list_certificates`
4. To list your policies, type the command: `python SetupAWS.py list_policies`
5. To list your things, type the command: `python SetupAWS.py list_things`

**Note**
You have to fill configure.json only for the `setup` and `cleanup` options.