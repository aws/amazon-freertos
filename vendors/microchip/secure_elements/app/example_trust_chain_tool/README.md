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

1) Build the demo application and program the target device
2) When the device boots is will export the pem formatted public key to the
uart

```
-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEojEQk85EaT1RU3Ip5SddaSqB5/Wm
+Vnxtu96G3i+gQRb8tb5xylXTXHQawL68SPW4/oCXXS4x7KGV0MNPneB6g==
-----END PUBLIC KEY-----
```

3) Save this key as public_key.pem for use in the device certificate creation

4) Run the ca_create_device script:

```
> ca_create_device.py  --sn 0123AE877AA4C660EE --cert signer-ca.crt --key signer-ca.key --file public_key.pem
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
demos/common/include directory of the demo project.

### Run the provisioning

With the client_credential_keys.h file now saved build and load the firmware to
the target device. Upon boot the call to vDevModeKeyProvisioning will correctly
provision the secure element. 

Once a connection has been established and proven
the call to vDevModeKeyProvisioning and the aws_dev_mode_key_provisioning.c 
source file can be safely removed from the project. This will save code space
and speed up the boot process reflecting the production configuration loading
credentials directly from the provisioned secure element.







