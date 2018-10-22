#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM "Paste client certificate here."

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate 
 * Authority) of the client certificate is used by the server for routing the 
 * device's initial request. (The device client certificate must always be 
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html, 
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 * 
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM \
"-----BEGIN CERTIFICATE-----\n" \
"MIICYjCCAgigAwIBAgIJAMZAFGod2v5GMAoGCCqGSM49BAMCMHcxCzAJBgNVBAYT\n" \
"AkRFMSEwHwYDVQQKDBhJbmZpbmVvbiBUZWNobm9sb2dpZXMgQUcxEzARBgNVBAsM\n" \
"Ck9QVElHQShUTSkxMDAuBgNVBAMMJ0luZmluZW9uIE9QVElHQShUTSkgVHJ1c3Qg\n" \
"WCBUZXN0IENBIDAwMDAeFw0xNjA1MTAyMDE4MzBaFw00MTA1MDQyMDE4MzBaMHcx\n" \
"CzAJBgNVBAYTAkRFMSEwHwYDVQQKDBhJbmZpbmVvbiBUZWNobm9sb2dpZXMgQUcx\n" \
"EzARBgNVBAsMCk9QVElHQShUTSkxMDAuBgNVBAMMJ0luZmluZW9uIE9QVElHQShU\n" \
"TSkgVHJ1c3QgWCBUZXN0IENBIDAwMDBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IA\n" \
"BJSJLwnqTsq8ak7yBjYm4F3g1fl36sOycKziGQD121bnN7u+RuRJdjgltfiUdJ4a\n" \
"tlrxKdc6tpuArMXhwxDyFsajfTB7MB0GA1UdDgQWBBRC411W5WyOjQJxjJ7yM8lH\n" \
"O4JTbDAfBgNVHSMEGDAWgBRC411W5WyOjQJxjJ7yM8lHO4JTbDASBgNVHRMBAf8E\n" \
"CDAGAQH/AgEAMA4GA1UdDwEB/wQEAwICBDAVBgNVHSAEDjAMMAoGCCqCFABEARQB\n" \
"MAoGCCqGSM49BAMCA0gAMEUCIQD18ytbkwmSkCykX3RWwSS7K5zkT8fw8Ww/X4FT\n" \
"nwl3mAIgUbCCd4UGd97vPUkht5Idh7XCkm2RB50C6mMcqOmRJaY=\n" \
"-----END CERTIFICATE-----\n";

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM "Paste client private key here."

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS 
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
