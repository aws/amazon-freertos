# Getting Started
This folder hosts the source code for a go based echo server. This can be used as a tool for testing TCP on FreeRTOS.
The TCP tests can be found [here](https://github.com/aws/amazon-freertos/blob/master/libraries/abstractions/secure_sockets/test/iot_test_tcp.c). See further documentation in the [porting guide](https://docs.aws.amazon.com/freertos/latest/portingguide/afr-echo-server.html).
## Requirements
1. OpenSSL
1. Golang
## Prequisites
### Credential Creation
#### Server
The following openssl command can be used to generate self-signed server certificate:
```bash
openssl req -newkey rsa:2048 -nodes -x509 -sha256 -out certs/server.pem -keyout certs/server.key -days 365 -subj "/C=US/ST=WA/L=Place/O=YourCompany/OU=IT/CN=www.yours.com/emailAddress=yourEmail@your.com"
```
#### Client
The following openssl commands can be used to generate client certificate:
```bash
openssl genrsa -out certs/client.key 2048

openssl req -new -key certs/client.key -out certs/client.csr -subj "/C=US/ST=WA/L=Place/O=YourCompany/OU=IT/CN=www.yours.com/emailAddress=yourEmail@your.com"

openssl x509 -req -in certs/client.csr -CA certs/server.pem -CAkey certs/server.key -CAcreateserial -out certs/client.pem -days 365 -sha256
```
## Folder Structure
The default location to store certificates and keys is the relative path to a folder "certs", update this in the configuration if you wish to move your credentials to a different folder.

The source code for the echo server is found in echo_server.go.

# Server Configuration
The echo server reads a JSON based configuration. The default location for this JSON file is './config.json', but if you wish to override this, you can specify the location of the JSON with the '-config' flag.

The JSON file contains the following options:
1. verbose
    1. Enable this option to output the contents of the message sent to the echo server.
1. logging
    1. Enable this option to output log all messages received to a file.
1. secure-connection
    1. Enable this option to switch to using TLS for the echo server. Note you will have to complete the credential creation prerequisite. 
1. server-port
    1. Specify which port to open a socket on.
1. server-certificate-location
    1. Relative or absolute path to the server certificate generated in the credential creation prerequisite.
1. server-key-location
    1. Relative or absolute path to the server key generated in the credential creation prerequisite.
## Example Configuration
```json
{
    "verbose": false,
    "logging": false,
    "secure-connection": false,
    "server-port": "9000",
    "server-certificate-location": "./certs/server.pem",
    "server-key-location": "./certs/server.pem"
}
```
# Running the Echo Server From the Command Line
`go run echo_server.go`
## With a Custom Config Location
`go run echo_server.go -config={config_file_path}`

Note: If you wish to run the unsecure and secure TCP tests at the same time, make sure you start a secure and unsecure echo server, this will require changing the configuration (You can create a second "secure" configuration, and pass it to the echo server via the -config flag.), as well as using seperate TCP ports.

# Client Device Configuration
Before running the TCP tests on your device, it is recommended to have already read through the [FreeRTOS getting started guide](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started.html).

You have to alter these 3 files:

* {amazon-freertos-root-directory}/tests/include/aws_clientcredential.h
* {amazon-freertos-root-directory}/tests/include/aws_clientcredential_keys.h
* {amazon-freertos-root-directory}/tests/include/aws_test_tcp.h

After following the prerequisite steps, you should have the following files:
* certs/client.csr
* certs/client.key
* certs/client.pem
* certs/server.key
* certs/server.pem
* certs/server.srl

1. In aws_clientcredential.h, you define:
	* The broker end-point
	* The thing-name
    * Access to WiFi (If not on ethernet.)

1. In aws_clientcredential_keys.h you define:
    * define keyCLIENT_CERTIFICATE_PEM to the contents of certs/client.pem
    * leave keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM as NULL
    * define keyCLIENT_PRIVATE_KEY_PEM to the contents certs/client.key
    * See this guide if you are unsure how to do [this](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-configure.html).
3. In aws_test_tcp.h :
    * define tcptestECHO_HOST_ROOT_CA to the contents of certs/server.pem
    * The IP address + port of the echo server:
        * tcptestECHO_SERVER_ADDR[0-3]
        * tcptestECHO_PORT
    * The IP address + port of the secure echo server:
        * tcptestECHO_SERVER_TLS_ADDR0[0-3]
        * tcptestECHO_PORT_TLS
