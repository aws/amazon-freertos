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
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWjCCAkKgAwIBAgIVANQFVEEX1rdV2kNagd5d+GpH8azBMA0GCSqGSIb3DQEB\n" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xODAxMDQxNzM2\n"\
"NTNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDx8glQ+/9kL009MKfp\n"\
"O6FB1H2hvw8EaGxvykgI1gjd6SWQUS43IPXz2DRrYQSdp4KoEXRzfH3po0KfD6zv\n"\
"cUiTw4ovBUt0l9/YR4YNeJn+g0P1xyW0VYAdVolAzPEWRU4WHXt/9DdIt/7IbbS3\n"\
"IzWeBGyL6CG6fA4xuza16LeqGyxiotMH3fO76qm4KeV2eSv3PR9P/Xl1WcbSlr5O\n"\
"ByxfU+ZvGfycVcXIHj9+/NEei5fDrGo9Y7fVWvTgsKg0FLf8MKpFrwyuKgVc7XFo\n"\
"EFifQ/YBXXBhUSNe/0NWJ6dt4oxvWJ13i54ysQGwrjP5UkMJiZTrhzJUw0UJkGWg\n"\
"Iv8pAgMBAAGjYDBeMB8GA1UdIwQYMBaAFAj23DXCwcg+8CU0lA+NLt+DZ7cUMB0G\n"\
"A1UdDgQWBBQRetIwEXcVR5YM9sGg6dTJwNK93jAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAZqFAAGkfEEaqaZXYCuaspMYq\n"\
"s6GcO5wmTC1vYJOkHK1Q7mWtGD7SqpXFquY5HwNbsfNJ1L96iav+HcovHvL3MRKW\n"\
"otOtWGvr+q6MphIYQbGjfhQrqTjvUIRTZ1VxLbK0ysZ1OxEtXO725YLtGEB/s4wt\n"\
"OHITvtWZIeDo1w0FaGdyQc6zEfgNbUFAEpE27A5DeWbKaOLhhJYYw/4ws3CJLj04\n"\
"ilB0ISvzwUTGgl4HoOXJHuV17Kxcz7hj46a8MYSSIOEg2eoNwMgsWnzWqHjmIh2x\n"\
"+7W6BRaXb3N+LOhbc4Irf9HRlL352cquYyDgR+cGy/SRrqzd1+phCgSpE1H6JQ==\n"\
"-----END CERTIFICATE-----\n"

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
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEA8fIJUPv/ZC9NPTCn6TuhQdR9ob8PBGhsb8pICNYI3eklkFEu\n"\
"NyD189g0a2EEnaeCqBF0c3x96aNCnw+s73FIk8OKLwVLdJff2EeGDXiZ/oND9ccl\n"\
"tFWAHVaJQMzxFkVOFh17f/Q3SLf+yG20tyM1ngRsi+ghunwOMbs2tei3qhssYqLT\n"\
"B93zu+qpuCnldnkr9z0fT/15dVnG0pa+TgcsX1Pmbxn8nFXFyB4/fvzRHouXw6xq\n"\
"PWO31Vr04LCoNBS3/DCqRa8MrioFXO1xaBBYn0P2AV1wYVEjXv9DVienbeKMb1id\n"\
"d4ueMrEBsK4z+VJDCYmU64cyVMNFCZBloCL/KQIDAQABAoIBAF43g7s+tsCbzvdC\n"\
"KFEiAHTzO40uKrnrtDaATjbqqn9IGHGvu1CKjFzN1/PszvBZ1/589Z03YI0IH5xz\n"\
"bUO9GKc6IWiYJj8cGXZOu/wo98bt9RwT1jH4jbBfD4nlhQjIKlVgW8SDJLgztor0\n"\
"dchPdMS5Mk/1MJU/VAO3xOFn44Xg1zMu+y9vLg3j5F8BpYJSBrcodYoGdLmTDjLi\n"\
"XUEKBIRCAEER84z4oN6AeWgJcV8U0ovhnkh9MpEZw4RILfyrIer3jMo+sYEX5gi5\n"\
"iQ/C/lon7zfGD+jgqPI3CkcFga+NbtEVLb/X/FR/m9/KUXAIXXRmEsY7rL3w2stD\n"\
"BC3WWhUCgYEA/hi790X5iCYKnXDV+cC2bRXY3hJI4lsfxYxciczJ3501M7v32bMo\n"\
"blQrFG4GQPj2iU9CjB8VBmbFjJPejPCze1Ot28s+rWmiICB9TrF+/Inw8uGGA/wj\n"\
"9YHYzHj0mtVatBsg15r6/xgKVa9S7MTqOHBgaCcC3MPWGnOIQMoyBxcCgYEA88IA\n"\
"Jx2kSZb/yxalvPIujEW9Up6sun3JOB05punzKl6RSVqbrNt5GNQQI3e7VhKFgI9Q\n"\
"njrrv0gfIuKqiq207eoMbHU1Pf9F6RoOZWfvQiG4OxcKonZxrqwwddH6GKwo9/OP\n"\
"Q7rSzdLMySlJsQw+lB1xm05B62cARym/4vGfE78CgYEA7r0+nm5YrdtQTe0enrK6\n"\
"tHO6d6ymXkdP6sSejbqkUj3bJuiNTzZN3pumfmOB7Ln+qbSWyiDKGGGnaxXkHHhr\n"\
"qoxM5h5onXaKpbThB3ct4QmaIhpTWJsnDh0aHcPnzzJ3LpHuoaUz/0oPK8K3H09E\n"\
"4hJ0AmflAJfgKqoycu2HY7ECgYAGLYuNcj3gs5gGEKvgrw6o9XeH6YedLmT0LCFy\n"\
"7okAMOdjwY0ig2l9TJqgG3++/bcue/NiH0IsVeqcGryRyTdFtGHeOdZAnQCrmMJ2\n"\
"pXy1417k+H+HgJL/B1QvGERLbcmYBy9BWzHOj6IrEwXQG+xWGqzvb7tR+QlTxGYd\n"\
"nVwXxwKBgBAW/91B2peTNbBUPbZMoM9SpLf/SMZwGzZI74FKjuJLeIiJCktW5zW0\n"\
"WsYFOo74KEgLwpxL/v92gprOQBoHk7TFpQ8OLmDwQW4Rz/W8nYIrXqRY7Wk6okMi\n"\
"a/dK1ONgEgcurvaF32mYz+SCGBFJ3Zx31nwW7iisrUVH5e1me8S6\n"\
"-----END RSA PRIVATE KEY-----"

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
