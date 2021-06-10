# OTA PAL Testing

## Enabling the OTA PAL tests
Each platform in this repository has a demo and a test project. You need to enable the OTA PAL tests in the test project before you can build and run them. This can be done by setting the `testrunnerFULL_OTA_PAL_ENABLED` macro in `aws_test_runner_config.h` to `1`.
* Test project configuration files directory: `<root-directory>/vendors/<vendor-name>/boards/<board-name>/aws_tests/config_files/`
* Test runner configuration file path: `<root-directory>/vendors/<vendor-name>/boards/<board-name>/aws_tests/config_files/aws_test_runner_config.h`

For example:
* `#define testrunnerFULL_OTA_PAL_ENABLED              1`

## Setting up the OTA PAL tests

Provision your device with the one of the certificates in `<root-directory>/tests/integration_test/ota_pal/test_files/` before running the OTA PAL tests. After provisioning your device, include the corresponding header file inside of `<root-directory>/vendors/<vendor-name>/boards/<board-name>/aws_tests/config_files/aws_test_ota_config.h`. The list of available testing certificates and corresponding header files can be found in the list below:

1. RSA with SHA1 certificate, was signed with the accompanying root CA certificates.
   * Root CA certificate: `rsa-sha1-root-ca-cert.pem.test`
   * Certificate: `rsa-sha1-signer.crt.pem.test`
   * Corresponding header file: `aws_test_ota_pal_rsa_sha1_signature.h`

1. Self signed SHA256 with RSA certificates.
   * Certificate: `rsa-sha256-signer.crt.pem.test`
   * Corresponding header file: `aws_test_ota_pal_rsa_sha256_signature.h`

1. Self signed ECDSA with SHA256 signatures.
   * Certificate: `ecdsa-sha256-signer.crt.pem.test`
   * Corresponding header file: `aws_test_ota_pal_ecdsa_sha256_signature.h`

For example, if you provision your device with `ecdsa-sha256-signer.crt.pem.test`, then you should include the following line in your platform's copy of `aws_test_ota_config.h`:
`#include "aws_test_ota_pal_ecdsa_sha256_signature.h"`
