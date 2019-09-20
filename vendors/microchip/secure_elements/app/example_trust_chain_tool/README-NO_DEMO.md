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
NOTE: THESE INSTRUCTIONS ARE FOR TESTING PURPOSES ONLY AND ARE NOT MEANT TO BE USED IN PRODUCTION!

1) Generate a public-private keypair for the device credentials.
openssl ecparam -name secp256r1 -genkey -noout -out deviceKey.pem

2) Isolate the public key from the private key
openssl ec -in deviceKey.pem -pubout -out devicePublicKey.pem

3) In the test project, copy the function pcPkcs11GetThingName() into project main.c.
(I put it immediately after vDevModeKeyProvisioning).  Put a break point in this function on line 193 of iot_pkcs11_secure_element.c  --> look in the memory and copy out the 18 digit "Thing Name" expected.

4) Run the ca_create_device script providing the public key (--file=devicePublicKey.pem --sn=Number From Step 3) and the serial number.

I got impatient trying to figure out why the syntax for my inputs was not working, so I modified the script directly like so, and then called py ca_create_device.py

   parser.add_argument('--file', default='devicePublicKey.pem', help='Input Public key or Certificate Signing Request')
    parser.add_argument('--sn', default='0123C6D3534196ABCD', help='Device Serial number if --file provided is a public key')


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

### Create the client_credential_keys.h file

Fill out \tests\include\aws_client_credential.h with your endpoint, your thing name (which is the serial number from above), WiFi credentials if necessary.

Using tools/certificate_configuration/CertificateConfigurator.html, provide device.crt for the Certificate PEM File, and deviceKey.pem for the Private Key PEM file.

Copy the produced aws_clientcredential_keys.h to replace tests\include\aws_clientcredential_keys.h 

### Run the provisioning

With the client_credential_keys.h file now saved build and load the firmware to
the target device. Upon boot the call to vDevModeKeyProvisioning will correctly
provision the secure element. 

Once a connection has been established and proven
the call to vDevModeKeyProvisioning and the aws_dev_mode_key_provisioning.c 
source file can be safely removed from the project. This will save code space
and speed up the boot process reflecting the production configuration loading
credentials directly from the provisioned secure element.







