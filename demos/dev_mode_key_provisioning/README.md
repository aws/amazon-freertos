# Developer-Mode Key Provisioning #
## Introduction ##
The purpose of the FreeRTOS developer-mode key provisioning demo is to provide embedded developers with options for getting a trusted X.509 client certificate onto an IoT device for lab testing. Depending on the capabilities of the device, various provisioning-related operations may or may not be supported, including onboard ECDSA key generation, private key import, and X.509 certificate enrollment. In addition, different use cases call for different levels of key protection, ranging from onboard flash storage to the use of dedicated crypto hardware. This demo provides logic for working within the cryptographic capabilities of your device.

## Option #1: Private Key Import from AWS IoT ##
For lab testing purposes, if your device allows the import of private keys, you can follow the instructions in [Configuring the FreeRTOS Demos](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-configure.html). 

## Option #2: Onboard Private Key Generation ##
If your device does not allow the import of private keys, or if your solution calls for the use of [Just-in-Time Provisioning](https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html) (JITP), you can follow the instructions below.

### Initial Configuration ###
First, perform the steps in [Configuring the FreeRTOS Demos](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-configure.html), but skip the last step (that is, don't do *To format your AWS IoT credentials*). The net result should be that the *demos/include/aws_clientcredential.h* file has been updated with your settings, but the *demos/include/aws_clientcredential_keys.h* file has not. 

### Demo Project Configuration ###
Open the Hello World MQTT demo as described in the [Getting Started guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting-started-guides.html) for your board. In the project, open the file *aws_dev_mode_key_provisioning.c* and find the definition keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR, which is set to zero by default. Set it to one:

```
#define keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR 1
```

Then build and run the demo project and continue to the next section.

### Public Key Extraction ###
Since the device has not yet been provisioned with a private key and client certificate, the demo will fail to authenticate to AWS IoT. However, the Hello World MQTT demo starts by running developer-mode key provisioning, resulting in the creation of a private key if one was not already present. You should see something like the following near the beginning of the serial console output:

>**Note**: Depending on the configuration value of `keyprovisioningDELAY_BEFORE_KEY_PAIR_GENERATION_SECS`, the device will wait for a delay period before generating the key-pair and printing the
device public key to the serial console.

``````
7 910 [IP-task] Device public key, 91 bytes:
3059 3013 0607 2a86 48ce 3d02 0106 082a
8648 ce3d 0301 0703 4200 04cd 6569 ceb8
1bb9 1e72 339f e8cf 60ef 0f9f b473 33ac
6f19 1813 6999 3fa0 c293 5fae 08f1 1ad0
41b7 345c e746 1046 228e 5a5f d787 d571
dcb2 4e8d 75b3 2586 e2cc 0c
```

Copy the six lines of key bytes into a file called *DevicePublicKeyAsciiHex.txt*. Then use the command-line tool xxd to parse the hex bytes into binary:

```
xxd -r -ps DevicePublicKeyAsciiHex.txt DevicePublicKeyDer.bin
```

Use openssl to format the binary encoded (DER) device public key as PEM:

```
openssl ec -inform der -in DevicePublicKeyDer.bin -pubin -pubout -outform pem -out DevicePublicKey.pem
```

Don't forget to disable the temporary key generation setting you enabled above. Otherwise, the device will create yet another key pair, and you will have to repeat the previous steps:

```
#define keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR 0
```

> **NOTE**: For boards that reset the device before flashing a new image, it is possible that the provisioned credentials become stale due to unexpected re-generationg of keys when flashing
a new image, that has the `keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR` configuration disabled, on the board. To mitigate this race condition between flashing a new image and re-execution
of existing image that generate new key-pair, there exists a delay before logic of key-pair generation is executed on device. You can configure the default delay of **30 seconds** by setting
the `keyprovisioningDELAY_BEFORE_KEY_PAIR_GENERATION_SECS` configuration macro.

### Public Key Infrastructure Setup ###
You can now use a variation of the instructions in [Use Your Own Certificate](https://docs.aws.amazon.com/iot/latest/developerguide/device-certs-your-own.html) to create an X.509 certificate hierarchy for your device lab certificate. Stop before executing the sequence described in the section Creating a Device Certificate Using Your CA Certificate. Also, since the device will not be signing the certificate request, a separate "issuance officer" key and certificate must be created for that purpose:

```
openssl genrsa -out issuanceOfficer.key 2048
openssl req -new -key issuanceOfficer.key -out deviceCert.csr
```

Once your Certificate Authority has been created and registered with AWS IoT, use the following command to issue a client certificate based on the device CSR that was signed by the issuance officer:

```
openssl x509 -req -in deviceCert.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out deviceCert.pem -days 500 -sha256 -force_pubkey DevicePublicKey.pem
```

### Certificate Import ###
With the certificate issued, the next step is to import it into your device. You will also need to import your Certificate Authority (CA) certificate, since it is required in order for first-time authentication to AWS IoT to succeed when using JITP. In the *aws_clientcredential_keys.h* file in your project, set the `keyCLIENT_CERTIFICATE_PEM` macro to be the contents of *deviceCert.pem* and set the `keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM` macro to be the contents of *rootCA.pem*. 

Recompile your project after the header change described above. Then re-run the MQTT Hello World demo in order to connect to AWS IoT with your new certificate and private key. Please note that the authentication attempt will again fail - see the next section.

### Device Authorization ###
The *Enable Automatic Registration* section of [Use Your Own Certificate](https://docs.aws.amazon.com/iot/latest/developerguide/device-certs-your-own.html) describes how to create a Lambda that will automatically mark your new device certificate as ACTIVE in the AWS IoT registry. If you are not using a Lambda, you must create a new Thing, attach the PENDING certificate and a policy to your Thing, and then mark the certificate as ACTIVE. All of those steps can be performed manually in the AWS IoT console. 

Once the new client certificate is ACTIVE and associated with a Thing and policy, run the MQTT Hello World demo again. This time, the connection to the AWS IoT MQTT broker will succeed.
