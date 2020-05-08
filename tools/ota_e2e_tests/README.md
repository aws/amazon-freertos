# OTA E2E Test Instructions

## AWS Account setup

1. Setup your AWS account for OTA: https://docs.aws.amazon.com/freertos/latest/userguide/ota-prereqs.html

## System Setup

1. This script currently uses Python 3, please make sure you have the latest python installed: https://www.python.org/downloads/.
2. Install the required python libraries:
`pip3 install pyopenssl pyserial junit-xml boto3`
3. Install the AWS CLI: https://aws.amazon.com/cli/
4. Configure the AWS CLI from the terminal:
    `aws configure`
    * If you don't have your **AWS Secret Access Key** saved on your machine, then get the **AWS Access Key ID** and the **AWS Secret Access Key** with https://docs.aws.amazon.com/general/latest/gr/managing-aws-access-keys.html
    * Set the **Default region name** as the same region you will create OTA update jobs from, example: `us-west-2`
    * Set the **Default output format** as `json`

## Board Configuration Setup

The test scripts need information unique to your board and your AWS account. This is contained in a **board.json**.

1. See **board.json** in the same directory as this README for information on each field. Create one or more for each board you wish to run tests on.
Some items are pre-filled, please verify the configuration is sufficient to your needs.

2. Some tests are designed to check that un-trusted certificates do not work. If you do not want to generate your own throw-away certificate/key pairs,
example certificates are provided, for signing schemes using RSA SHA256 and ECDSA SHA256, under the 'credentials' directory. The names of the files
indicate the signing algorithm. Import the certificates with the algorithm applicable to your device into AWS Certificate Manager. Use the resulting ARN for
the `"aws_untrusted_signer_certificate_arn"` board config value.

## Running the tests

* Run all of the tests on board specified in board.json:
`python ota_e2e.py --board-configs board.json`

* Run all of the tests on directory of board configurations:
`python ota_e2e.py --board-config-dir C:/example_dir/board_configs/`

* Run only the specified tests:
`python ota_e2e.py --board-configs board.json --tests OtaTestGreaterVersion`
The names of the available tests are pre-filled in board.json under the `"supported_tests"` field.

* More options specified in --help:
`python ota_e2e.py --help`

## FAQ

### How to create a self-signed code signing certificate

1. Make sure [openssl](https://www.openssl.org/) is installed and can be accessed from commandline.
1. Use the following text to create a file named `cert_config.txt` in your workspace. Replace
   `test_signer@amazon.com` with your email address.

    ```txt
    [ req ]
    prompt             = no
    distinguished_name = my dn

    [ my dn ]
    commonName = test_signer@amazon.com

    [ my_exts ]
    keyUsage         = digitalSignature
    extendedKeyUsage = codeSigning
    ```

1. Create an ECDSA code-signing private key,

   `openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -pkeyopt ec_param_enc:named_curve -outform PEM -out ecdsasigner.key`

1. Create an ECDSA code-signing certificate,

   `openssl req -new -x509 -config cert_config.txt -extensions my_exts -nodes -days 365 -key ecdsasigner.key -out ecdsasigner.crt`

   **Note: to change the exipration date, modify the value of `-days` option.**

1. For RSA private key and certificate,

   `openssl req -x509 -config cert_config.txt -extensions my_exts -nodes -days 365 -newkey rsa:2048 -keyout rsasigner.key -out rsasigner.crt`

For more information, check the AWS document, https://docs.aws.amazon.com/freertos/latest/userguide/ota-code-sign-cert.html
