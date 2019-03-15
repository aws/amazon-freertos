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
#define keyCLIENT_CERTIFICATE_PEM                                        \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUbarN+FUOAxmhL5KV/wxH6x8tqs4wDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDIyODE5NTUw\n" \
"NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALfwV9ab3ME137zSmWI2\n" \
"KdZW+LqVT4Svu9XVTqMQBuhmtort0EL6eRlQ8wAoDizoLyq6xE4qQz+UFldGQ8qo\n" \
"h72/wxf2/MkXQn4EzGYxLfVaCVQogoClrpTHg1tFKFGqqqyiwsEcphMgiH9VvHSb\n" \
"YztS3gLlWkJW3X1IieGjC1JSn3tLfzXAMxWwp4h0Q/kJDj0bLtwWHzmAWZxdOxGw\n" \
"p+NGz3rfkHTA2XFrtGMNd5gwKHHn1YttQ60IH53R6OtNwPvrSzPYqen44SLUIRfA\n" \
"xTxla9zXu7Ya9eLuEk18a92Gqv89nXy4FEGTcDTMtMjMfSA0+13f8n25bKwsXzfd\n" \
"7aUCAwEAAaNgMF4wHwYDVR0jBBgwFoAUyrEuTSsQ/QwrLV66dj7B5GG4LUIwHQYD\n" \
"VR0OBBYEFFB7zXK/iDVZZEdDcfHOD0XhoGYYMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAlJuCMWzm2kfgZVPMiK6w1Z08+\n" \
"uTdZv5iHlp9rqV1UBIcXuzdxAU2k4Z8+LYO6De7Zc9DEaHM7Egmu+SEMfoh/lnzi\n" \
"/udeuGTnOIDdxVDsN2e0aSW5BNOHRKYU4TQQmYA17/O0RLv5S1MXTVAbeFfZk/W9\n" \
"qCHpuci8hMny4fzVFdWYVrihwPB+/Fs9JGI76fcqkEsh5nb3BNGiOSN8/z0RcN++\n" \
"c0Kwz61mtwRBsheLj5pLa+wekJCxnOJvxQsqyiw4uY+eHozcZQKO0vCdRiioKM4S\n" \
"Z7GajEd5fLV2Xo6gMAELIf9F1PY4MdXMKNcOvAV6cfFEU3vh6Ua1RDu8fis8\n" \
"-----END CERTIFICATE-----\n";
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
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM    NULL

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM                                        \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEAt/BX1pvcwTXfvNKZYjYp1lb4upVPhK+71dVOoxAG6Ga2iu3Q\n" \
"Qvp5GVDzACgOLOgvKrrETipDP5QWV0ZDyqiHvb/DF/b8yRdCfgTMZjEt9VoJVCiC\n" \
"gKWulMeDW0UoUaqqrKLCwRymEyCIf1W8dJtjO1LeAuVaQlbdfUiJ4aMLUlKfe0t/\n" \
"NcAzFbCniHRD+QkOPRsu3BYfOYBZnF07EbCn40bPet+QdMDZcWu0Yw13mDAocefV\n" \
"i21DrQgfndHo603A++tLM9ip6fjhItQhF8DFPGVr3Ne7thr14u4STXxr3Yaq/z2d\n" \
"fLgUQZNwNMy0yMx9IDT7Xd/yfblsrCxfN93tpQIDAQABAoIBAQCFClfeJ/nx3a+o\n" \
"eYLroFV+bIWXHuvXAuyPa6tnEe6NJc2eiwf5igURJJW+ZTprjLyg4VmfV74wPEI0\n" \
"eeQKrv8PZaYMrV6FbEdxsrLMr4GYVrksn+hUeIHfprcByvGwsAqmt0q6NqiAaSWR\n" \
"REf8529Mt1FRMuan2QB234KrKyzDKzeXYlo6j5mPud+LF4uQt+DjrRZkAwUZpvzD\n" \
"3Zc/jT50lXtNMR1/fajaZ+71GxCk/5qdgVhgiKPoKyDcsnkZVDxRhJO4G+1VlzEf\n" \
"LWkkNSqAXV4tdOFduG2cA+h3IjduZOuU9Uux5XofV7XaIewc0jUJ+wnPBKsH48Mj\n" \
"fOwl3u9BAoGBAPPWhZ0PL7bH4ECJ7hF7Px3b7AOkgtoT9Ldkls3lNroXlsbQ6Mg3\n" \
"zCEHYKA3n3WSeVM9hWhJuU9ga/PIXBM9U8c1GtQF9wcsdvMAZJAAtFTI/UW+lfQN\n" \
"Sc6aYRp9A6fVx+gifGR5eYd4aF1z2WMuZjB88jU2j8Ed/fVkGgZ+MFbZAoGBAMEc\n" \
"/bIJekRK24rWm5dGt2J0LwnxgAQscmvciQEwCbyIO/moSZz0TPQqkixZFh6fpun1\n" \
"Zxfy/Sbghv7IZ7q4haX4n/CSEJHvp6Iq3mK7s27raML641RaYSt+VSUIkdUVBJoh\n" \
"rDX11WvkQja+iD6s47wdwddDLYgNAx2ipoUU6QWtAoGBALe/DozeZsxMLpcz2Wze\n" \
"/R8ZHO9ZxyljJGQWrvWVJvpw0DQeNWy7b13sW0og4E8PA9JQZ/nApJ5GQNNCRvBQ\n" \
"pJNXV1Iy9IY5YaGYyC8j+MBzfThTeFiist23xDjbwCd3l988LZThXU1S1QKnTEfL\n" \
"LUKYxUSISyfx2/BRcLM34GFxAoGAe8v3AtQ6kl8ewLF1SxQlabMrmIc91u8bZvQY\n" \
"zC8cfsUjFZgBoOGoF3rW/CEcIgOdACy2q061ZN/ZCqo0fSnfW5e4CIPKpZNp9nbe\n" \
"25KSo8+7+ArYAvJxim1Rbg1BV8VCjwB5Ipp299R7chbQxsUUtTqqUjIZlTcqQSxM\n" \
"7dGK1H0CgYBJG1BDDvl9sud5kGOTqZQWSgAaONVDS9ib0p141oW7+eKwbITtFitN\n" \
"uACkU07nrjTyz2YimWUgB21r/ZeGA2CoSegGxa8aDp/49MKTsa3RONNd0mKlniHo\n" \
"+o00HmVKKNlyPH2uwwoeBDAtkJGEbbqNTa4HjN9/P3jteLOol80pNQ==\n" \
"-----END RSA PRIVATE KEY-----\n";

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char * clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
