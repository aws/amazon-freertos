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
"-----BEGIN CERTIFICATE-----" \
"MIIDWTCCAkGgAwIBAgIUSWYwKSwBH2ikCow7VywL8inmpA0wDQYJKoZIhvcNAQEL" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE4MDkxNzE2NDA0" \
"MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL894dMV7fH0sMr4GAib" \
"wfRipxYZ9+Qc31nvLmohNGJzjDA5eS7t0GbTp2xPSqxozMCSVn7yPxr3papLW9k0" \
"58angGtEK5CqI4e4bW0jNfa3djnuH/QPsMbtgFByydWbfsBz6C8ifYKBc8rhVcyu" \
"QLrRhI1VXHL3iOHzOnTaVs8mL+CY00cu8VWjuYVDMICqdfGdeqoJs00JULgDQplw" \
"Ixoh4ZOHdUJAClpc5Nwq2pmS7marwhY6MU/cBP9XVSqetOboUkBIehH4jxqK5+DE" \
"7dNNnWTmZXO3G32YOG3Zkd/4ZRhOtbx3GuByv6AKP0IQDqV931E8QB4RuZW2nq0i" \
"M8kCAwEAAaNgMF4wHwYDVR0jBBgwFoAUvqb7lBM+plvALtinG/EW0zpHRTUwHQYD" \
"VR0OBBYEFI22K2v7PiZo/kyCHexhl5O4korAMAwGA1UdEwEB/wQCMAAwDgYDVR0P" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBFddfeAGH+HLT0MNNGoiLcWfMt" \
"U+LACIzuD2J9XwXkKHOMSRGjbqNz5Sgs+1b5Q533Tpsk4fu1a7tNCYAPJJPlbEqd" \
"5Sg6hEkO9PFMnMrmBN+gIdXbCnk57ij6R3F9dvjMLfx/KO5eJV4C9JKGNAWTTVEJ" \
"lq52XTSA44YVB97728S+8WC7Z+IPMAaMtbhiMXBIVIiuDUbstOTVO+TriixvG3Lo" \
"oAXyh7ItNSRshqnWyNV/VWw32mowm/CPdruYUH7EOLT6xgfqq0i/eYxdKQsffxLc" \
"DLus/Qa/gA2PpFqbJaaKuvaBNMLulPc4Whe0Ye97HBvlqqQg9C4e0BODQJ1X" \
"-----END CERTIFICATE-----"


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
"-----BEGIN RSA PRIVATE KEY-----" \
"MIIEowIBAAKCAQEAvz3h0xXt8fSwyvgYCJvB9GKnFhn35BzfWe8uaiE0YnOMMDl5" \
"Lu3QZtOnbE9KrGjMwJJWfvI/Gvelqktb2TTnxqeAa0QrkKojh7htbSM19rd2Oe4f" \
"9A+wxu2AUHLJ1Zt+wHPoLyJ9goFzyuFVzK5AutGEjVVccveI4fM6dNpWzyYv4JjT" \
"Ry7xVaO5hUMwgKp18Z16qgmzTQlQuANCmXAjGiHhk4d1QkAKWlzk3CramZLuZqvC" \
"FjoxT9wE/1dVKp605uhSQEh6EfiPGorn4MTt002dZOZlc7cbfZg4bdmR3/hlGE61" \
"vHca4HK/oAo/QhAOpX3fUTxAHhG5lbaerSIzyQIDAQABAoIBACmOZ+D3lfIKytQA" \
"HzDXr8jMGyS5Vd2SzJH9YGedeYWfOCpOgt/Iu41slqf5fu+CAmQiKHuDZmUUxUIB" \
"CB9l2KuZQFWOxay02+2lDNo3WYWYDi++Z+Ef+7juaQCSQPuSw3NMfHqpkv5l5OSE" \
"HKHSh6xQ1O3lS6aL1YnL0bsMqupuMLQsfQhC3laSxtXMB6IHUNT9d69mKod49Ost" \
"5a1n+0v1c9K8Co1/8x7MoFAQPZ9GClDXRFjEJliywzZC4Dn5c1/cuige6G4fzVIb" \
"K7Yw93SzOcmLH3aYzgp3rINBTWxXOl+19bpb0SM3TIoGbYsKYc8Y6sbRYINIzvLv" \
"O0DZVBUCgYEA39EpYXc1JP9H1KX+BRpokMX4l2a71SsJiOhlPgY8wniIDVt52R6M" \
"330zYmtHjrr+sWnSgVxjxUjECHyYLeOLyk5Mvf6lJ2LIaGgu1AuFKG2ANJO7ttXo" \
"uYzD4RlzBZgUt/Nyn4J00BbDOwuExKTOpwbPpDuSjJEL06RvcerG+H8CgYEA2r2a" \
"mhVwFtdM92U2uhN1q1py62iW8Ui2FrlwF1GchqbxwTuPm+6WxZJ5roGhvhlwfaaE" \
"IcCR28VaP52eoCcZlE3uuBFe3AW9WMMkA0U2CMdY6htEOetnp5yiPdvNFyLK6QGM" \
"X0bDJowOHQfyla2Y5eBjhhGB8yzKUhei2bUY77cCgYBXWUEGGcjy5HI7KoElqrXO" \
"coTfSafavM4e0VvZxgNmfjXSyLWYJ9Ugz7MDIQ0vRglqYftWoyEI+KoRCdc2aYy/" \
"6qkGqV/5LBM8tBQ6izhvm7SiUFm9NGR8LdSwgAck0sKbO/arC/Xc1FVjMM6jFBLW" \
"mtDkrp82HtF3yr/3vIHkrwKBgQDG2tXAvDqBWUusIMRm1ygDFiAhKJ0ITxzSfdSM" \
"LnJI614eWAJPNNC2XlnmrgyXYFMmt4mpkYJINBq+eJFrcQBNFaguiAQfq8wd9JE5" \
"s+LSij+n3A/jQ8Gg28GW0Mqa/NNQjkrJlu/0jCCMfdwn/IBiguKXUDU+F/r6NGik" \
"2IEJ8wKBgD22ePu5CIGJmRtO36bYUDqbPYHMsPbycf5vb8wlzWYACSMMjVNHIaZI" \
"JJ58iKHySmNiGsFaQNzsM/mpNUeAo9GgRrnKpnogIKYRhe33vchP7sPjTRIQKKSt" \
"M96JeNrossevYcQqpNnG4dUCkAS+Z9d+zfGTbtzTUMZtY2e5nZkf" \
"-----END RSA PRIVATE KEY-----"


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
