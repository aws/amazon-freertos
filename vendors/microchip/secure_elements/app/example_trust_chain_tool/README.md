# Microchip Secure Element Tools

These tools are used to set up an example chain of trust ecosystem for use with
Microchip ATECC508A and ATECC608A parts. Included are utilities to create the
ecosystem keys and certificates.

## Dependencies

Python scripts will require python 3 to be installed. Once python is installed
install the requirements (from the path of this file):

```
> pip install -r requirements.txt
```

The application will have to built by loading the microchip_security_tool.sln
project file and building either the x86 or x64 version of the application

## Set up a Certificate Ecosystem

The first step is to set up a certificate chain that mirrors how a secure iot
ecosystem would be configured. For this we'll create a dummy root certificate
authority (normally this would be handled by a third party, or an internal
PKI system) and an intermediate certificate authority that mirrors the signing
hardware security modules (HSM) that are used in the Microchip facility during
manufacturing of security devices.

### Create the Root CA

```
> ca_create_root.py 
```

### Create the Signing CA

```
> ca_create_signer.py <code_from_aws>
```

code_from_aws is the registration code required to verify the private key used
for the signing certificate. This is obtained where you upload the signer
into the AWS IOT cacertificatehub. 

1) In the AWS console navigate to AWS IOT -> Secure -> CAs
2) Click the "Register" button and then the "Register CA" button
3) Copy the registration code in "Step 2"
4) Run ca_create_signer.py with the registration code
5) Select the CA certificate (default is signer-ca.crt)
6) Select the Verification certificate (default is verificationCert.crt)
7) Select Activate CA certificate & Enable auto-registration
8) Click the "Register CA certificate" button
9) Make note of the certificate ARN hash for later (the first few digits)

The intermediate signing certificate is now active and allows for automatic
registration of new devices when they are created (requires lambda functions).
At this time we are not going to configure the lambda functions for automatic
registration so we'll upload the devices certificate manually.

### Create the Device Certificate

1) In the demo, navigate to aws_dev_mode_key_provisioning.c.  Ensure that a fresh
keypair will be generated for this process.
```
#define keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR   1
```
2) Build the demo application and run the code.
3) When the device boots is will export the ECC608a device serial number and 
the DER formatted public key to the console.

```
6 2005 [IP-task] Recommended certificate subject name: CN=01234D2C14CBEAD5EE
7 2007 [IP-task] Device public key, 91 bytes:
3059 3013 0607 2a86 48ce 3d02 0106 082a
8644 ce3d 0301 0703 4200 0424 3557 251f
d8b8 de2f 80f8 f743 2929 dab9 140e c69d
89dc ef44 9fc4 f99e 053b 13b3 1f12 5c3c
0f04 5055 1234 2127 ffdf 5678 abf0 cd66
3d85 ee41 255a 57e0 a393 56
```

4) Copy the six lines of key bytes into a file called *DevicePublicKeyAsciiHex.txt*. Then use the command-line tool xxd to parse the hex bytes into binary:

```
xxd -r -ps DevicePublicKeyAsciiHex.txt DevicePublicKeyDer.bin
```

5) Use openssl to format the binary encoded (DER) device public key as PEM:

```
openssl ec -inform der -in DevicePublicKeyDer.bin -pubin -pubout -outform pem -out public_key.pem
```

6) Don't forget to disable the temporary key generation setting you enabled above. Otherwise, the device will create yet another key pair, and you will have to repeat the previous steps:

```
#define keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR 0
```
7) Run the ca_create_device script, using the 18 characters following CN= for the --sn input:

```
> ca_create_device.py  --sn 01234D2C14CBEAD5EE --cert signer-ca.crt --key signer-ca.key --file public_key.pem
```

This step mirrors the production provisioning process where Microchip uses HSMs
to sign each device produced and loads the certificate information into them.
This performs the certificate creation and signing process but does not load the
certificates into the device (provisioning).

If lambda functions have been set up for Just In Time Registration this following
step may be skipped, otherwise we have to load the device certificate into the
aws certificatehub manually.

1) In the AWS console navigate to AWS IOT -> Secure -> Certificates
2) Click the "Create" button
3) Select "Use my certificate" path by clicking the "Get started" button
4) Select the CA that was created earlier (see the ARN hash we noted)
5) Click "Register Certificates"
6) Select the device certificate (default device.crt)
7) Click "Register certificates"
8) Find the newly create "inactive" certificate and select it
9) Attach Policies and Things to the new certificate
10) Activate the certificate

The certificate is now ready to be used by the aws_demos_secure_element examples

### Export the client_credential_keys.h file

> ca_write_header.py

This creates the file that is used by AWS FreeRTOS to save the generated
certificates into the secure element. This file needs to be copied to the
demos/common/include directory of the demo project to replace the existing
blank file.

### Update aws_clientcredential.h file
Update clientcredentialMQTT_BROKER_ENDPOINT[] and clientcredentialIOT_THING_NAME using
the instructions located in the section "To configure your AWS IoT endpoint"
https://docs.aws.amazon.com/freertos/latest/userguide/freertos-configure.html
(Do not follow instructions in other sections on this page.)

### Run the provisioning

With the aws_clientcredential.h and aws_clientcredential_keys.h file now 
saved build and load the firmware to the target device. Upon boot the call to
vDevModeKeyProvisioning will correctly provision the secure element. 

Once a connection has been established and proven
the call to vDevModeKeyProvisioning and the aws_dev_mode_key_provisioning.c 
source file can be safely removed from the project. This will save code space
and speed up the boot process reflecting the production configuration loading
credentials directly from the provisioned secure element.







