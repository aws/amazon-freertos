#OTA PAL Testing

Please provision your device with the correct certificates in \<root-directory\>/tests/integration_test/ota_pal/test_files before running any OTA PAL tests( these tests are enabled by setting testrunnerFULL_OTA_PAL_ENABLED to 1, see aws_test_runner_config.h ).

The names of the files correspond to the signing algorithm associated with that certificate.

If your OTA PAL layer port defines "otapalconfigCODE_SIGNING_CERTIFICATE" variable in ota_pal.c, then please copy in the certificate, with the applicable signing algorithm for your device, located under \<root-directory\>/tests/integration_tests/ota_pal/test_files. Currently the "otapalconfigCODE_SIGNING_CERTIFICATE" variable is set to use the ecdsa-sha256-signer.crt.pem certificate located under \<root-directory\>/tests/ota_pal/test_files by default.

## OTA PAL Device Certificates File information

Self signed SHA256 with RSA certificates.
   File: **rsa-sha256-signer.crt.pem** - Certificate
   File: **rsa-sha256-signer.key.pem** - Private key

This RSA with SHA1 certificate, was signed with the accompanying root CA certificates.
   File: **rsa-sha1-root-ca-cert-key.pem** - Root CA private key
   File: **rsa-sha1-root-ca-cert.pem** - Root CA certificate
   File: **rsa-sha1-signer.crt.pem** - Certificate
   File: **rsa-sha1-signer.key.pem** - Private key

Self signed ECDSA with SHA256 signatures.
   File: **ecdsa-sha256-signer.crt.pem** - Certificate
   File: **ecdsa-sha256-signer.key.pem** - Private key
