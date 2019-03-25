## Getting Started with the WM_W600_Arduino EV Board

If you don't have a WM_W600_Arduino EV Board, you can purchase one from our [partner](https://www.seeedstudio.com/W600-Arduino-IoT-Wi-Fi-Board-p-2926.html).
For more information about these boards, see WM_W600_Arduino EV Board on the [WinnerMicro website](http://www.winnermicro.com/en/html/1/162/163/535.html).



## Prerequisites
Before you get started with Amazon FreeRTOS on your board, you need to set up your AWS account and permissions.

To create an AWS account, see [Create and Activate an AWS Account](https://aws.amazon.com/premiumsupport/knowledge-center/create-and-activate-aws-account/).

To add an IAM user to your AWS account, see [IAM User Guide](https://docs.aws.amazon.com/IAM/latest/UserGuide/). To grant your IAM user account access to AWS IoT and Amazon FreeRTOS, attach the following IAM policies to your IAM user account:

- AmazonFreeRTOSFullAccess

- AWSIoTFullAccess

### To attach the AmazonFreeRTOSFullAccess policy to your IAM user

1. Browse to the [IAM console](https://console.aws.amazon.com/iam/home), and from the navigation pane, choose **Users**.

2. Enter your user name in the search text box, and then choose it from the list.

3. Choose **Add permissions**.

4. Choose **Attach existing policies directly**.

5. In the search box, enter **AmazonFreeRTOSFullAccess**, choose it from the list, and then choose **Next: Review**.

6. Choose **Add permissions**.


### To attach the AWSIoTFullAccess policy to your IAM user

1. Browse to the [IAM console](https://console.aws.amazon.com/iam/home), and from the navigation pane, choose **Users**.

2. Enter your user name in the search text box, and then choose it from the list.

3. Choose **Add permissions**.

4. Choose **Attach existing policies directly**.

5. In the search box, enter **AWSIoTFullAccess**, choose it from the list, and then choose **Next: Review**.

6. Choose **Add permissions**.

For more information about IAM and user accounts, see [IAM User Guide](https://docs.aws.amazon.com/IAM/latest/UserGuide/).
For more information about policies, see [IAM Permissions and Policies](https://docs.aws.amazon.com/IAM/latest/UserGuide/introduction_access-management.html).



## Setting Up the W60X Hardware

See the [WM_W600_Arduino EV Board](http://www.winnermicro.com/en/upload/1/editor/1552985563187.pdf) Getting Started Guide for information about setting up the WM_W600_Arduino EV Board development board hardware.
        **Note**
        Do not proceed to the Get Started section of the WM_W600_Arduino EV Board guides. Instead, follow the steps below.


## Setting Up Your Environment

### Establishing a Serial Connection
To establish a serial connection between your host machine and the WM_W600_Arduino EV Board, you must install CH304 USB to UART Bridge VCP drivers.
For more information, see Establish Serial Connection with W60X. After you establish a serial connection, make a note of the serial port for your board's connection. You need it when you build the demo.
The **default serial port baud rate** is 115200.

### Install MDK
The W60X demos takes the MDK development environment as an example and needs to install [MDK-ARM5](http://www2.keil.com/mdk5/install/).



## Download and Configure Amazon FreeRTOS
After your environment is set up, you can download Amazon FreeRTOS from GitHub.

### Downloading Amazon FreeRTOS
Clone or download the amazon-freertos repository from [GitHub](https://github.com/WinnerMicro/amazon-freertos).
        **Note**
        The maximum length of a file path on Microsoft Windows is 260 characters. Lengthy Amazon FreeRTOS download directory paths can cause build failures.
        In this tutorial, the path to the amazon-freertos directory is referred to as *BASE_FOLDER*.

### Configure the Amazon FreeRTOS Demo Applications

1. If you are running macOS or Linux, open a terminal prompt. If you are running Windows, open **mingw32.exe**.

2. To verify that you have Python3 or later installed, run **python --version** . The version installed is displayed. If you do not have Python3 or later installed, you can install it from the [Python website](https://www.python.org/downloads/).

3. You need the AWS CLI to run AWS IoT commands. If you are running Windows, use the **easy_install awscli** to install the AWS CLI in the mingw32 environment.

If you are running macOS or Linux, see [Installing the AWS Command Line Interface](https://docs.aws.amazon.com/cli/latest/userguide/installing.html).

4. Run **aws configure** and configure the AWS CLI with your AWS access key ID, secret access key, and default region name. For more information, see [Configuring the AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-getting-started.html).

5. Use the following command to install the AWS SDK for Python (boto3):
- On Windows, in the mingw32 environment, run **easy_install boto3**.
- On macOS or Linux, run **pip install tornado nose --user** and then run **pip install boto3 --user**.

Amazon FreeRTOS includes the SetupAWS.py script to make it easier to set up your WM_W600_Arduino EV Board to connect to AWS IoT. To configure the the script, open *<BASE_FOLDER>*/tools/aws_config_quick_start/configure.json and set the following attributes:

afr_source_dir
        The complete path to the amazon-freertos directory on your computer. Make sure that you use forward slashes to specify this path.

thing_name
        The name that you want to assign to the AWS IoT thing that represents your board.

wifi_ssid
        The SSID of your Wi-Fi network.

wifi_password
        The password for your Wi-Fi network.

wifi_security
        The security type for your Wi-Fi network.
Valid security types are:
- eWiFiSecurityOpen (Open, no security)
- eWiFiSecurityWEP (WEP security)
- eWiFiSecurityWPA (WPA security)
- eWiFiSecurityWPA2 (WPA2 security)

### To run the configuration script

1. If you are running macOS or Linux, open a terminal prompt. If you are running Windows, open mingw32.exe.

2. Go to the *<BASE_FOLDER>*/tools/aws_config_quick_start directory and run python SetupAWS.py setup.

The script does the following:
- Creates an IoT thing, certificate, and policy
- Attaches the IoT policy to the certificate and the certificate to the AWS IoT thing
- Populates the aws_clientcredential.h file with your AWS IoT endpoint, Wi-Fi SSID, and credentials
- Formats your certificate and private key and writes them to the aws_clientcredential.h header file

For more information about SetupAWS.py, see the README.md in the *<BASE_FOLDER>*/tools/aws_config_quick_start directory.



## Build and Run the Amazon FreeRTOS Demo Project

### To configure your board's connection for flashing the demo

1. Connect your board to your host computer.

2. Take MDK5 as an example to open the project: *<BASE_FOLDER>*/demos/wm/w60x/mdk_proj/helloworld.uvprojx.

3. Click the Build button to generate the firmware, the firmware is located *<BASE_FOLDER>*/demos/wm/w60x/bin/WM_W600_GZ.img (or WM_W600.FLS).

4. Execute the script to burn the firmware into the flash, the script is located *<BASE_FOLDER>*/lib/third_party/mcu_vendor/wm/w60x/Tools/download.py.


        python download.py [COM] [image]


You can use the MQTT client in the AWS IoT console to monitor the messages that your device sends to the AWS Cloud.

### To subscribe to the MQTT topic with the AWS IoT MQTT client

1. Sign in to the [AWS IoT console](https://console.aws.amazon.com/iotv2).

2. In the navigation pane, choose **Test** to open the MQTT client.

3. In **Subscription topic**, enter **freertos/demos/echo**, and then choose **Subscribe to topic**.

Hello World *number* and Hello World *number* ACK messages appear at the bottom of the MQTT client page when the terminal or command prompt where your WM_W600_Arduino EV Board returns output.

    12 1350 [MQTTEcho] Echo successfully published 'Hello World 0'
    13 1357 [Echoing] Message returned with ACK: 'Hello World 0 ACK'
    14 1852 [MQTTEcho] Echo successfully published 'Hello World 1'
    15 1861 [Echoing] Message returned with ACK: 'Hello World 1 ACK'
    16 2355 [MQTTEcho] Echo successfully published 'Hello World 2'
    17 2361 [Echoing] Message returned with ACK: 'Hello World 2 ACK'
    18 2857 [MQTTEcho] Echo successfully published 'Hello World 3'
    19 2863 [Echoing] Message returned with ACK: 'Hello World 3 ACK'

When the demo finishes running, you should see output similar to the following in your terminal or command prompt:

    32 6380 [MQTTEcho] Echo successfully published 'Hello World 10'
    33 6386 [Echoing] Message returned with ACK: 'Hello World 10 ACK'
    34 6882 [MQTTEcho] Echo successfully published 'Hello World 11'
    35 6889 [Echoing] Message returned with ACK: 'Hello World 11 ACK'
    36 7385 [MQTTEcho] MQTT echo demo finished.
    37 7385 [MQTTEcho] ----Demo finished----


## Troubleshooting
ALL W60X-based development boards can refer to the SDK documentation on the [WinnerMicro website](http://www.winnermicro.com/en/html/1/156/158/497.html), they are universal.