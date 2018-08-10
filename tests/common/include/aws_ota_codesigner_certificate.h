/*
 * Amazon FreeRTOS
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
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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


#ifndef __AWS_CODESIGN_KEYS__H__
#define __AWS_CODESIGN_KEYS__H__

/*
 * PEM-encoded code signer certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */

/* Currently ecdsa-sha256-signer.crt.pem is assigned to this variable. If needed, please
 * please replace with the certificate applicable to your board under tests/common/ota/test_files
 * before runing any tests. */
static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBXDCCAQOgAwIBAgIJAPMhJT8l0C6AMAoGCCqGSM49BAMCMCExHzAdBgNVBAMM\n"
    "FnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wHhcNMTgwNjI3MjAwNDQyWhcNMTkwNjI3\n"
    "MjAwNDQyWjAhMR8wHQYDVQQDDBZ0ZXN0X3NpZ25lckBhbWF6b24uY29tMFkwEwYH\n"
    "KoZIzj0CAQYIKoZIzj0DAQcDQgAEyza/tGLVbVxhL41iYtC8D6tGEvAHu498gNtq\n"
    "DtPsKaoR3t5xQx+6zdWiCi32fgFT2vkeVAmX3pf/Gl8nIP48ZqMkMCIwCwYDVR0P\n"
    "BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0cAMEQCIDkf\n"
    "83Oq8sOXhSyJCWAN63gc4vp9//RFCXh/hUXPYcTWAiBgmQ5JV2MZH01Upi2lMflN\n"
    "YLbC+lYscwcSlB2tECUbJA==\n"
    "-----END CERTIFICATE-----\n";

#endif /* ifndef __AWS_CODESIGN_KEYS__H__ */
