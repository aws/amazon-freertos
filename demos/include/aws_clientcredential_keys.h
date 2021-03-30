/*
 * FreeRTOS V201906.00 Major
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
"MIIDWjCCAkKgAwIBAgIVAMlmnOgTlmFRzg/8H685jE65b4MEMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTAzMzAxNTAx\n"\
"MjJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDIQkeCAIAAmXmS+09N\n"\
"BBQDcfY65qCu90Bg0GsgA7MPpu7PdaXmbxOrUwMAJQrZj547WnXJAiG0b/0O6fkm\n"\
"RkiXBIp/WxW9fP+XQoiLCAvcmrLct3IwCH4JISuhnP2HLTCuDSD0VU80BzseGjd4\n"\
"JuT28zDyR29a0AAyw6NFjFIVDStpl8pXCxsuQRRwuJyc/lh3KZ9M8T5zSDgomtrH\n"\
"Vmu47Uyy62Vi3Cia0ixCCiG9AlChxQmsnkB05qgXgVLGqpXO+sL0N/YfJPcNGMWn\n"\
"MuouK5vlq4CkqcFTW67KdZs89OvmXiaHfkfPl6eFRyqTMT+5pGl6dILUkg4H80kK\n"\
"pw0zAgMBAAGjYDBeMB8GA1UdIwQYMBaAFE+dgvB7Y60N2IHNwMbd6iAsEZYOMB0G\n"\
"A1UdDgQWBBSCUHjysmt9w9ONeT8i3FYemT7vEjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAZ2cZstZj2HDeyK+UcUU9iuHb\n"\
"eU6d3BIXTlBvGmDb2h8eh9ozVmaPOyx0cxwgfd0JKfWmIDX1D+qDbWUnMAnft/Kv\n"\
"9Hynw1CIjoMq31thHiv2oDsZrlR5+HwaCyogfLE7T/f2nWPR/EKHFREDwQNUXyMg\n"\
"t0w2i6R911p5K2lrE4Va80t+lF3Syh93a9grjCiIT3zcEbzsNx1ecg7xH2BqAtSO\n"\
"HjseGCK2Zu94b1hlDoHhzGpmAfTH7ShP1QYIX2bqJKtguFg7gB3o4nte+/eXLC+C\n"\
"/yjhIzgXl9wJSvjTHPrmFx1K1HL4gOrmCoFwRuIVb3SzF4l8O63cCfXDjouJDA==\n"\
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
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAyEJHggCAAJl5kvtPTQQUA3H2OuagrvdAYNBrIAOzD6buz3Wl\n"\
"5m8Tq1MDACUK2Y+eO1p1yQIhtG/9Dun5JkZIlwSKf1sVvXz/l0KIiwgL3Jqy3Ldy\n"\
"MAh+CSEroZz9hy0wrg0g9FVPNAc7Hho3eCbk9vMw8kdvWtAAMsOjRYxSFQ0raZfK\n"\
"VwsbLkEUcLicnP5YdymfTPE+c0g4KJrax1ZruO1MsutlYtwomtIsQgohvQJQocUJ\n"\
"rJ5AdOaoF4FSxqqVzvrC9Df2HyT3DRjFpzLqLiub5auApKnBU1uuynWbPPTr5l4m\n"\
"h35Hz5enhUcqkzE/uaRpenSC1JIOB/NJCqcNMwIDAQABAoIBAAVNM+VuK/VYGs7Y\n"\
"Vl4cmIcK0lD24BQhuZOWkO91CZ2FdtcJL04tbKDhfrfXQzVFRxQdLfcXVlsXh+SI\n"\
"Ly9jNvLAjQYYHDg7Ae04NrMvkFjTw55yQNSitOt69FO/+zpD6QECmfe99m3TLy55\n"\
"1TQaG3+qKoaPe5Q043x0j3L85wNlnM1h4V26jPPD7r2ZIZrTz0BSso6AHw7Rxrl8\n"\
"GmbqQcZ+959TpVTEVnUIkufbPdc+SB+znIVFE0BBbusvuthdOxY56OLVybFDB24U\n"\
"6jGGGE4EkMMepxqpl7NihUxkcMckSq3khlQjmyqIJIMw8NNt0jj1oRZXma0IOWdK\n"\
"rfJwkQkCgYEA+6sScKavnQWLZ9ER1etlL2YQu4x8u61Uf2PvSdD7Tef+VDg3xPhf\n"\
"kOo9o3mPrQ6Wyfq/0cBUFzteZFqXd4UuYuq798aHZeecnWMRPDDaW2tYw5hM3UyV\n"\
"61h93tvGAugNDRSbYeELZbz4q+9NQv878K2OfOz57P8ENXOK6UiFrAcCgYEAy7Su\n"\
"jdT2Uf6svyL9gX+qQ8D8IpQ8zUuCeGXSF/fhumdTSZayWWIgC5wQ7aTln0B6FE2k\n"\
"1dHKTjYJo/83KxET0Ghzc93D9lCCfmNNSafhXLmMDZ1H4dvigBvilKFCrX41p+dF\n"\
"6GgxLZkkNtoT3vcqCSQCvE7Q5k2sxG/NjvT1onUCgYB8uuezv1kwSIw0EvF1BWqr\n"\
"2DkYyuMTHS/Qx1XUTugOT5nEYex6CeNLblzIRbpIuf9P8Z/aNCk5Lmp08FU7nl1E\n"\
"FzS788AtaGI9utjS7R97ybM/lZ8a2xc+gig4KNJ1pE67mef81c2RSdwxUrQHLXqm\n"\
"qU4Camzf4fBTEYXaYuAlRwKBgB/nNE9SQyGkWTm2KS3W6LujxdlEjiE46j4g9D58\n"\
"vAWNmSW1TEljbNFkp+IQKUY/ut7jKiWyrZSCMhBVMDa2ffvt3+cNnFs/ULCqrLgU\n"\
"at8y0BRxxYhB/79AnsPYXbD+Szj4p6I2bz1M9rcAgIwy+Ypk8Wi8j0sCb0R0niT9\n"\
"iFrhAoGBAIe5pNpRJILQ/b2cNFTrHaRk6VnKFmDSamqfMpqZ4jaim9CPgnw6pE/v\n"\
"Lbw+nBFY+ypgvv7r6Xkq5d/1+4UnC+f4WWgIcsVZDa0DW7FvWny8K7O9nXT8gDIA\n"\
"oW5bvBvztQ2wNZczSrLaX4NkofeVDqzcz5m7Fg2D1nl2djo5z4tI\n"\
"-----END RSA PRIVATE KEY-----\n"


#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
