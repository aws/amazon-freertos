/*
 * Amazon FreeRTOS V201906.00 Major
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the Amazon FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM                   "-----BEGIN CERTIFICATE-----\n" \
"MIIDWjCCAkKgAwIBAgIVAPhWx7zRuzpDIXPlwKdZ4/fjQVOSMA0GCSqGSIb3DQEB\n" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTA3MTgxMTI3\n" \
"MDRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDc2zrL8MPTnf/Jmoet\n" \
"/806viYmrFYH7SJF2MQowjkTfsHTNdtSeVEyStp30qqzaBaROokAVSMycX4eKXQW\n" \
"843uyJHF89lk55vQVx+IzqrESGBtOQ3WYzdRvR8qly9b+nCwNiQqRE56cGJsTCz4\n" \
"+vzAJuJ/4FZwi8ihDrPjTVdskGyhSVOOGYO1VAY+ksmwd/mRzdiczbUqz1qJguPs\n" \
"AKo0dE5YDfqX7V6QXN9Ba/vHfXhb5Z1pJzB8h/d4u0upaqG1Cmmlh31wGX6b17Su\n" \
"22xGwR/adk5jNnrWjUyx7VgLOMMwL17iu+ygTUoYy1MYoDLDw9EnyiKpJE3gXzNv\n" \
"h+AJAgMBAAGjYDBeMB8GA1UdIwQYMBaAFEd9krZ2HtR9hh8DYk8nMZNo0cxJMB0G\n" \
"A1UdDgQWBBR4hEs48K+SRskVjIZw8KYtKIjJsjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAeCPjOA1JjFIkNTMSA9+A1jq/\n" \
"kq6XRqjLVSKIAUCA2HeWWIkXb4ohC+3bRXiTU5k2P25NsUtUNb+zLFCaziKqtMFz\n" \
"cfUhmBV4J7GZaKtKdHzqwFWBBnf26XGr1qM8C77vngO0ufHGNM8qczFCsneAfMv4\n" \
"KhwrrBdWQMJsbBp+IuotTlS7w6XEaBEQfR2od/syG26jogHHBfCzAohkwANUqIWw\n" \
"GSxIwvFcqj1+YYyeHl7j5KoijCqGNB9MfsSi+b6FhrCW5BkTKM4qmj20Gzas7JH6\n" \
"YU1b+/ab6rvcMkYS4y6MQ2LrHMnkTtYIjZcG3BgLVeeloFF0R6EbiiCluE/aiQ==\n" \
"-----END CERTIFICATE-----"

/*
 * @brief PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 *
 * @todo If you are using AWS IoT Just in Time Registration (JITR), set this to
 * the issuer (Certificate Authority) certificate of the client certificate above.
 *
 * @note This setting is required by JITR because the issuer is used by the AWS
 * IoT gateway for routing the device's initial request. (The device client
 * certificate must always be sent as well.) For more information about JITR, see:
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
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM    ""

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the Amazon FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM                    "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEA3Ns6y/DD053/yZqHrf/NOr4mJqxWB+0iRdjEKMI5E37B0zXb\n" \
"UnlRMkrad9Kqs2gWkTqJAFUjMnF+Hil0FvON7siRxfPZZOeb0FcfiM6qxEhgbTkN\n" \
"1mM3Ub0fKpcvW/pwsDYkKkROenBibEws+Pr8wCbif+BWcIvIoQ6z401XbJBsoUlT\n" \
"jhmDtVQGPpLJsHf5kc3YnM21Ks9aiYLj7ACqNHROWA36l+1ekFzfQWv7x314W+Wd\n" \
"aScwfIf3eLtLqWqhtQpppYd9cBl+m9e0rttsRsEf2nZOYzZ61o1Mse1YCzjDMC9e\n" \
"4rvsoE1KGMtTGKAyw8PRJ8oiqSRN4F8zb4fgCQIDAQABAoIBAHgEcpktB13SOJQ3\n" \
"849suPw5JMZTVJUOGlfWihKUKSwc7/t1AvB4mNU5tCr1QmooIEtfgd+6ZFwfLKf7\n" \
"YuiuBe1C9WSbNQIOkq/PCIXxuP7dTflMzeY5GHDE01Dzw3q6R86aPWDyqasDs5ii\n" \
"dLYAvqU0/FT9djvG2oobUUouV1nYx2Fj1jAxJ70neknbFnBtZyLG7DccrJPs84bh\n" \
"bBCu3Ky48/Xqp4AcVKzqseEOaGM5tYBQ3LmAKvJbpWygLtHZCGjJrbJS/BCvPHb+\n" \
"Z2tqzOOrC78N+jI1x9hIFM8JOXCGnGoswC4lEVxBct+bRc4yBL+wtxVfgqS35P9q\n" \
"mmI5IfkCgYEA+HBkDLLLNyn7OA3Xn87vYzGQWFKD6V6lq9dqzXUfGxwVsaSOtRMA\n" \
"i5GbeZKA5XZG7ATGLmOZnj2q8V7bgaq07NSh8e3zNNmX+L+i62Hn4wxsNvW1W5Us\n" \
"UlGdtfz1EU/GRfzQ5j9+FB/FYLYRH7E5bE7qdxsYKEe7i1j7hx7XKusCgYEA45Pw\n" \
"pso7QDUJbFIhw//IVdp/F+czLy1SW2oiMAKrlI+q2FervepzxP96ks//C6q/l8ii\n" \
"z9Rdc0oZ94HRcgaWz1SVN8Gx/yfigxHLiLHJG/MgQNjz46pJK2TkOTlGevaet+h5\n" \
"YZmsHbNNa6Rw0EwLZGllGU/qNDNbm4fd2/s2O9sCgYEAr8XVA6b8YGszctVYWZjv\n" \
"hh7rIZJAAj0UuiAPUI4tE+Eq+CSVU0e+ewDqH3mn+LFWPGakUO3nIi4/Y8bZR0Sf\n" \
"/W/zpFfWWu4Yt8XeXutGRT3k0n3FcqZ4VUWpb7kpKrZmcd4Nbto9Ob/AxkwKkYGy\n" \
"Pw8WlG53CQi3iD/Mtiz7z0ECgYEAuXlE1GpVR8rx1xpI8phwCah9lFmVWYmyaVZi\n" \
"boZ+mCFOt+sbyThfCdq9ZxGlZzjN1UJrjr4hO2jVKObgb/H2zxq8UijGjJFRBklm\n" \
"fA2JPgIz8yXd/1o1L+yWJB0N2AFO8cvBwU2g/MO5AVWPq/OOBVPwVWTQF6MTsfpV\n" \
"Uoe2d6kCgYASYEwnYbkKbJ111/sVoDeKXL9M26kyX9PpDu+hRhTXJ7oHE4XY60V0\n" \
"C6k9/S9cRngjStKN2rzWeAmSZiZzl9wkUE0f1Pq2fSA86BUF29MUijq4ogtDL8Ev\n" \
"ukNEDPgm3NdiCua4YecxZSISU/HegtpMapGOQTd2iJnLm5xUYwz5FQ==\n" \
"-----END RSA PRIVATE KEY-----"

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
