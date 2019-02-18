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
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDWjCCAkKgAwIBAgIVAM3ttVS5h78ByJD0cdH9jYKuFu7cMA0GCSqGSIb3DQEB\n" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTAxMzAyMzE0\n" \
"NTFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC/w5OlnZSe9hC49Kls\n" \
"qF9pB6/W0Om6LQAg5WR0l17klz3XvNe25hxsO9i++/FtT8BlrYr217WFLNsjoyik\n" \
"CFI20zgM8o/HVGVknFS/kRAvu9nyR38ZvOI1gH+rhoek7q52/BbWm090Fx4jqlQ8\n" \
"fgrLix5A7GOy5VzAfZ2qEW61yzDRfbMGWgMjazUaY9eR97Zk45yudSj1Yi9lAnpk\n" \
"qG+uV27WCItpMd5R6XZqTU5jThYcrv8US18dBdpujtm444A2T3k9FhziUIttKpJp\n" \
"0W8z/Tw+he8uD+ikW50bLSJbVvUKLh0awBAqA54I5skZJYH14Wo1Ljzk4OcnVq4Z\n" \
"psKFAgMBAAGjYDBeMB8GA1UdIwQYMBaAFGWILoBY8qFMiyi78ItA5dR//JyJMB0G\n" \
"A1UdDgQWBBTsg+ABz4q+GsywZdUtiH96kuG2tTAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAEmB6FRwX88Wzl8sS/nrvgKW1\n" \
"BrJYSHrxbAn/nKqxFmyvTggNfVm3dZ7x4EKGUHMD9UFV27sKeUhW4a+ux/PIa2qx\n" \
"wngUc20pFx9TPyPgLOH7g9Dbi/fAAndERZJzXJaM6/sqR8Qhmss+c5JEQTmmTJKq\n" \
"n943/N/sKcvFcJJXANPheypOJaoxKOvYafW39GtjKk7GcpikbpbTUHkErSZEWYTl\n" \
"Jbix8auqJntrO633zQyIebPUumDKrTX/0XrbFM2mnFw5VhVJb9Ro+L7Ns+3b+i3/\n" \
"tN2BzVFvBHsdBKb3222OxK8d4VWK9rbVUxu9Cony2vNxFALuCIDvtgg7Dzb7Ww==\n" \
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
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"  \
"MIIEpQIBAAKCAQEAv8OTpZ2UnvYQuPSpbKhfaQev1tDpui0AIOVkdJde5Jc917zX\n" \
"tuYcbDvYvvvxbU/AZa2K9te1hSzbI6MopAhSNtM4DPKPx1RlZJxUv5EQL7vZ8kd/\n" \
"GbziNYB/q4aHpO6udvwW1ptPdBceI6pUPH4Ky4seQOxjsuVcwH2dqhFutcsw0X2z\n" \
"BloDI2s1GmPXkfe2ZOOcrnUo9WIvZQJ6ZKhvrldu1giLaTHeUel2ak1OY04WHK7/\n" \
"FEtfHQXabo7ZuOOANk95PRYc4lCLbSqSadFvM/08PoXvLg/opFudGy0iW1b1Ci4d\n" \
"GsAQKgOeCObJGSWB9eFqNS485ODnJ1auGabChQIDAQABAoIBAQCkq/ggIh+8GaXm\n" \
"jgLnkfbMpJM1LxVmkLophZMsI8iTF+U9W6cKIrSJpSLNbmC18+RN7CcHLdls0UYl\n" \
"ZjjCE3s2JU9envUkwK6JJU4JazKxKulPimH04iSGuR/pVY8p7OTYvEWIaLpkAG4b\n" \
"RmsrvzjZ7U8/hT9gImHx0m0P3ARsCt4ptkyYPuO2XAMwtDVHF5YrgVxAyy5N0+Q5\n" \
"KVBKLSOwvx/OwQmSstX3XNayLDjiLeGPSjHCiqq06g9Og1n6m+KrHN+rvjj106l9\n" \
"UvfrIs+V87rkQl5lN5Fg8Cj78B+/QMXC+Jaalf0NBQhR4UplU49RbeFIiim9Aihl\n" \
"z5EoYqjBAoGBAP+KjrahFzw3cq3srF82yVNO55unXuXrOAaQjZYZR4qFNC5bI+KJ\n" \
"TxNhlb5bD8cmj3yLPdNvsYk10wG6OhzIGd8hPChxrQ6GmSeUH744GMBg63o9WW4D\n" \
"o/M4TinDP+Xo0yIK1SJtVaf07hsKTLo4zFdaAlOCPy0hdIRI9EFyKXXNAoGBAMAb\n" \
"tVLGknbD8TTZrMfKlBpFpjtoKXP4u4ysHEYNLh8dD7UfRDho+ilj34g1zSFsVMIs\n" \
"xvdPR+MPbynHzA1T9lIE0eih3Jf17bTUvKyOGuz1UPJSjls67ydgDgYpXmddLbEV\n" \
"GTvNbYB+XZYta5hNpfBeur/OEyRKoj9L/l9ogceZAoGBAI2lWPwEiSOYvJmJsfZJ\n" \
"IQlrmFggSyr42iJRaBoGQzCWK1lyrY55e/qLQfy07yb9ydVIPWoA5JDg4+h2OJYR\n" \
"mL2k53qnoDyIAUb+BSZkY9vnHT+x1+tyWyOLV1Aw6S9ggf/Br2iY4DzsODuG1wb2\n" \
"StY0j0uIy8oRXEujQLkyNacVAoGBAJgqmnKplUP7qqiJLwHGJY+10ZlGxWb2A2DG\n" \
"x1J5iCne9B0sWfYwuM5mLJ4Qt54c1/pCS4uDQdbJ1+yJNMjt07Nd3ALfiwLXKuVM\n" \
"iPJJUWyWV/rJh7cdtGdE/3bD9+H5/YJByw4ChxT7gxe8fHhUMPd3oYbiLWlWsLNg\n" \
"d2mNNxcpAoGAW1hsU5s9ro38GkNoAX/8EbbkwvQjj8PgYQLhmBmY9QLta6i1UHmL\n" \
"ubIDAqiI6uJfdlzFyakdxe8aDY/Q/ZokGfy60mptHqbtpIpCdaWYh3rcMKwVPgrj\n" \
"14S4tarZtD7tmNG5jaeakTILR4hAM9JVQ67AMaZ/HUoz6vFeQlZjwto=\n"\
"-----END RSA PRIVATE KEY-----\n";


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
