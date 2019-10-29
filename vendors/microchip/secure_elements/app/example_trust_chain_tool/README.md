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

In this flow, you will create a Root CA, a Signing CA, and finally a device certificate
in what is known as a "chain of trust".

Your Root CA will be self-signed at the top of your chain of trust.  

Your Signing Certificate will be trusted by your Root CA, and will be used to issue your device certificate.

Your Device Certificate will be unique per device, and is trusted by your Signing Certificate.

### Create the Root CA
First, create your Root CA.  
```
>python ca_create_root.py 
```

### Create the Signing CA
Next, create your Signing CA.  This CA will be registered with your AWS IoT account.  
In order to prove your ownership of the Signing CA, obtain a registration code from AWS.

```
aws iot get-registration-code
```

This code will be used to generate a verification certificate, 
as well as a Signing CA certificate & keys.

```
>python ca_create_signer.py <code_from_aws>
```

Now register your CA certificate with AWS using the following command, passing in signer-ca.crt to
register that certificate with AWS, and verficationCert.crt to prove that signer-ca.key belongs to you.

```
aws iot register-ca-certificate --ca-certificate file://signer-ca.crt --verification-cert file://verificationCert.crt
```

This API returns the certificate ID for your new CA certificate (this is the 64 charcter hex value that follows arn:aws:iot:<region>:<account>:cacert/<ca-certificate-id>).

Your signing CA certificate is now "inactive", meaning that it cannot yet be used.  Activate it by 
calling 

```
aws iot update-ca-certificate --certificate-id <ca-certificate-id> --new-status ACTIVE
```
where <ca-certificate-id> is the ID you got back when you registered your signing CA.

Note that if you are using Just In Time Registration or Just In Time Provisioning, you will also need to enable auto-registration.
```
aws iot update-ca-certificate --certificate-id <ca-certifiate-id> --new-auto-registration-status ENABLE
```

### Create the Device Certificate
For each device that you want to register with AWS, you will need to generate
unique keys, and a corresponding certificate.

1) Go to the Amazon FreeRTOS code, and open the demo project, located at <amazon-freertos-root>\projects\microchip\ecc608a_plus_winsim\visual_studio\aws_demos.

In the demo, navigate to 
demos\dev_mode_key_provisioning\src\aws_dev_mode_key_provisioning.c.  
Ensure that a fresh keypair will be generated for this process.
```
#define keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR   1
```
2) Build the demo application and run the code.

3) When the device boots it will generate a private key (which never leaves the device),
as well as a public key. The public key and device serial number are exported from the 
ECC608a and printed to the console.

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
If you do not see this printed to the console, try power cycling your device, double
check that the file aws_clientcredential_keys.h has all defines set to "", and verify
that keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR is set to 1.

The 18 characters following the "CN" will become your device's "Thing Name".  The 91
bytes contain your device public key.

4) Copy the six lines of public key bytes into a file called *DevicePublicKeyAsciiHex.txt*. 
Then use the command-line tool xxd to parse the hex bytes into binary:

```
xxd -r -ps DevicePublicKeyAsciiHex.txt DevicePublicKeyDer.bin
```

5) Use openssl to format the binary encoded (DER) device public key as PEM:

```
openssl ec -inform der -in DevicePublicKeyDer.bin -pubin -pubout -outform pem -out public_key.pem
```

6) Don't forget to disable the temporary key generation setting you enabled above. 
Otherwise, the device will create yet another key pair, and you will have to repeat the previous steps:

```
#define keyprovisioningFORCE_GENERATE_NEW_KEY_PAIR 0
```
7) Create your device certificate using the ca_create_device script. 
Use the 18 characters following CN= for the --sn input, and the public_key.pem file
that you formatted step 5 as your public key.

```
>python ca_create_device.py  --sn 01234D2C14CBEAD5EE --cert signer-ca.crt --key signer-ca.key --file public_key.pem
```

You now have a device certificate called device.crt.

This step mirrors the production provisioning process where Microchip uses HSMs
to sign each device produced and loads the certificate information into them.
This performs the certificate creation and signing process but does not load the
certificates into the device (provisioning).

If lambda functions have been set up for Just In Time Registration this following
step may be skipped, otherwise we have to load the device certificate into the
aws certificatehub manually.

### Register with AWS IoT

1) Register your device certificate with AWS IoT.  
```
aws iot register-certificate --certificate-pem file://device.crt --ca-certificate-pem file://signer-ca.crt
```
This command lets AWS know that your device certificate is linked to your account, and that the Signer CA
you previously created is above your device certificate in the chain of trust.

The output is the certificate ID of your device certificate.  Take note of your certificate
ID, as it will be used later in the instructions when we attach a policy to it.

2) Activate your certificate 
```
aws iot update-certificate --certificate-id <device-certificate-id> --new-status ACTIVE
```

3) Create a policy called "DemoPolicy" for your thing's certificate.
Modify policy_template.json to restrict or expand the permissions of your device.
```
aws iot create-policy --policy-name DemoPolicy --policy-document file://policy_template.json
```

4) Attach the policy to your device's certificate.
```
aws iot attach-policy --policy-name DemoPolicy --target arn:aws:iot:<region>:<account-id>:cert/<device-certificate-id>
```
Remember to use the <device-certificate-id> generated in step 2 of this section.

5) Create a Thing - this is the digital representation of your device in AWS IoT.
Use the Thing Name from your serial number/CN value obtained while running the demo code.

```
aws iot create-thing --thing-name 01234D2C14CBEAD5EE
```

6) Attach your device certificate to your thing.  Remember to switch out the example Thing Name in the command below for your thing name from the previous step, and the region/account
id/device certificate id for your device certificate's ID.
```
aws iot attach-thing-principal --thing-name 01234D2C14CBEAD5EE --principal arn:aws:iot:<region>:<account-id>:cert/<device-certificate-id>
```

The certificate is now ready to be used by the aws_demos examples

### Export the client_credential_keys.h file
Next you will need to tell your device about the certificate you just created for it.
Generate a header file to include your device certificate and signer CA certificate
in your demo project.
```
>python ca_write_header.py
```

This creates the aws_clientcredential_keys.h file that is used by 
Amazon FreeRTOS to save the generated certificates into the secure element. 
This file needs to be copied to the demos/common/include directory of the 
demo project to replace the existing blank file.

### Update aws_clientcredential.h file
Update clientcredentialMQTT_BROKER_ENDPOINT[] and clientcredentialIOT_THING_NAME

Use the Thing Name from earlier in the instructions.  You can find your endpoint 
by calling
```
aws iot describe-endpoint
```


### Run the demos again

With the aws_clientcredential.h and aws_clientcredential_keys.h file now 
saved build and load the firmware to the target device. Upon boot the call to
vDevModeKeyProvisioning will correctly provision the secure element. 

Once a connection has been established and proven
the call to vDevModeKeyProvisioning and the aws_dev_mode_key_provisioning.c 
source file can be safely removed from the project. This will save code space
and speed up the boot process reflecting the production configuration loading
credentials directly from the provisioned secure element.







