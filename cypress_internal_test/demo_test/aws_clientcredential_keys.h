/*
 * Amazon FreeRTOS V201912.00
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUF88DxWtc4XkyrGB3saXbT2usaHUwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MTIyMDIzMjMx\n"\
"NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM5/Eyq4PjvNBbHdGgOr\n"\
"kBjYdakqBnEwXsQ0EUQquADaQSIph3ElLFQqC6c+hvLdQOrA5A02Whzr9La4wcJY\n"\
"C2v4lDA+erna2rTMiFd4e2cuodXw3/XScmw4q0sIyDkJfcMxlJ9dsOLIeDhdsqKj\n"\
"Yi0/iTWyJrcoN79G//lEcec2DO5L8jlwlVe8TZlJPWfelf8FTRYpzxZDTCVRTeCj\n"\
"Cf9FSQ6xYI41CQLIGaRDu0VuMi7/3CEwYJ9iK9v5Ai5C2v9YOQXT7DToWIsokeZJ\n"\
"HLk4h6PsViMMwQmMd/P+sYLLHNmINGIWh+eintlMi+VP0yp6LLsGkPI/hHaKKB4Y\n"\
"wHkCAwEAAaNgMF4wHwYDVR0jBBgwFoAUL7psqBsfB5jiiQROZKaZO7QxVGAwHQYD\n"\
"VR0OBBYEFAFYSm4ZX95YG9qYVoqNiCcZ4XMrMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQC/sgj80UIbsXaeO1vHgrSBJd+o\n"\
"GvKZFDRAGHktAADJnD2PpEHGjXZefvDSlW4jtmYMVNtQNQj24nDXrjVOLrZcgFpp\n"\
"CdIuRGJQzin1MQTJXIFKO0xUxkgIPdbYAb3DgVlUQff2XA2DqUCg8PQMZUpIkNss\n"\
"IZoc5vTeIV+rbSK/8Uuw73v6dxwKlOmgLm0+w8TMW6wrkPvyljn/5MZHTq2Dg9gK\n"\
"OFa/GeSavAmrQ9cdsG0EwBexAaB2aoRSeOAQbLbBPR9eUaN2blfu8zTvO6Rvq94u\n"\
"XlQuis6UQ0Ja0TX5Dp6uxwutAMqKI2j4LD0XpJcg3r8vXc4OAfGAgqZsujJ4\n"\
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
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  ""

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpQIBAAKCAQEAzn8TKrg+O80Fsd0aA6uQGNh1qSoGcTBexDQRRCq4ANpBIimH\n"\
"cSUsVCoLpz6G8t1A6sDkDTZaHOv0trjBwlgLa/iUMD56udratMyIV3h7Zy6h1fDf\n"\
"9dJybDirSwjIOQl9wzGUn12w4sh4OF2yoqNiLT+JNbImtyg3v0b/+URx5zYM7kvy\n"\
"OXCVV7xNmUk9Z96V/wVNFinPFkNMJVFN4KMJ/0VJDrFgjjUJAsgZpEO7RW4yLv/c\n"\
"ITBgn2Ir2/kCLkLa/1g5BdPsNOhYiyiR5kkcuTiHo+xWIwzBCYx38/6xgssc2Yg0\n"\
"YhaH56Ke2UyL5U/TKnosuwaQ8j+EdoooHhjAeQIDAQABAoIBAQCHQYcdcUcaRoWX\n"\
"Zjq28rZU/9u57VQA6ez42R4CJ3UU4eDN7qPPwciSbGPrrTIZfEEcQzJdUR+fWBKX\n"\
"aNdJZAA/PbhbDWG3wRmGWX5oxPsXr9zDVJ5t1uWp60iPwnXbnlLo+t3dsTG+qg7M\n"\
"5NqwWxJgjwBh0oo3aDlzqZ7a+DEffwK/DaY16AyZbNvIFM/NCB4D5UWWJEHcT/LW\n"\
"3FSdn/izkpEfy109GVn/V8iircGwPiHIG+K5j+PE91DZM0w2sHdWt2TO0foa1tcs\n"\
"MNff8AS/Cq98GxkbtKkdycVLye2/BPt/zabFE45YMcDokPsWs9fIUm9NnmRYJALD\n"\
"atJw9fsdAoGBAPjRht11zMAtlf2TQ1Kb0T4Cqd2lcOvDTUB+bym4I5oAkxwzsaMo\n"\
"/eKmclmMGHg0RIg+AV35C6KbX3x2qtQquHyeF/sz/AqQrTp6K51AZ7IC0XZ6O3pm\n"\
"e/TqFY65ekOzRaVAR7BD+rl7A4jvoaOTjYAIrsdMUv6CUfw4SyL/YpqTAoGBANR0\n"\
"1o8TGPwduw5H8PxQKh4rIMlJH/BvRYJ+8zNGtkDIoNc+4vcXLxq3L5nsrhPddA3F\n"\
"VJ1ELL/Y7EHRNcrEeKOAxrS1gWdMknXf2Qxg5OO5zJ5GbRAOeKPLViP3gkFH2EZa\n"\
"NzoxvSfERJ95N+hBcvI3YzC2uc9n8MJTgMH7oARDAoGBAK+leccsOXYQuClho8zV\n"\
"IkjcKH3HJIfKPuagWmjhDIDSlwb5lxTPAyrUxTRhqYRxoU9sNhrYjFYlyKpcOwu7\n"\
"fLC8CcHTWME8kI0PYWqI5KrNQoCYmRQ29JpYBQ/T73OLQkb9sZ3E32HAbSZlw31t\n"\
"1B3mAzu9rXjtFmMuehwinMDxAoGBAK0s/6dn/bmj0lF1uq0v+uTUaskn3ezyBkq0\n"\
"iDCHtw9feht5my+lqjOwlhB1p/9xR6Ph8XKvm3pVq16K1boOI/fChDXQrUYyWD/U\n"\
"0usQIAKv9GYfKc8ovJVmrTRsD7R+h3/xEYtR98luZjFHcZbTooplM2kFyLeTFeCQ\n"\
"iCeWn3xZAoGAUK2YrYJH80q8kcPUPwcnug9AAnKu2EcrFa1aTpGeNyZpa84djuCE\n"\
"CE81e8UAaofzQ0gbAqRKlXn8z+xSmDOs6eo/a+yW2mHaqDOl603/NvPas4x7HoY8\n"\
"JyXfGe9HT6wm7GZ2vZLij6hYnLzkugQCLPtKhGu2aEREldafBKuZn7E=\n"\
"-----END RSA PRIVATE KEY-----\n"

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
