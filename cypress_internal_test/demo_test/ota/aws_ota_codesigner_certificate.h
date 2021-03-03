/*
 * FreeRTOS V202002.00
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

#ifndef __AWS_CODESIGN_KEYS__H__
#define __AWS_CODESIGN_KEYS__H__

/*
 * PEM-encoded code signer certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"
 * "...base64 data...\n"
 * "-----END CERTIFICATE-----\n";
 */
static const char signingcredentialSIGNING_CERTIFICATE_PEM[] = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIBazCCARKgAwIBAgIUaQ9BWrOlkEBXzNIRCZrGeFyx4zMwCgYIKoZIzj0EAwIw\n"\
"IzEhMB8GA1UEAwwYcmF5bW9uZC5uZ3VuQGN5cHJlc3MuY29tMB4XDTIwMDQxNzIz\n"\
"NTAzNloXDTIxMDQxNzIzNTAzNlowIzEhMB8GA1UEAwwYcmF5bW9uZC5uZ3VuQGN5\n"\
"cHJlc3MuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEtgdImNKz5cFN5TtV\n"\
"m90eYEnjPfNT0VQE4EDt06b78XBPd8+Um5OMQrmqUju8EmJT9Bx/+H7D4Z6mvsu6\n"\
"+Z+8i6MkMCIwCwYDVR0PBAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqG\n"\
"SM49BAMCA0cAMEQCIBKEQ6oM7XnqHa1l7sWXsq7ANmiYQxyLFssohs0Yi60VAiAa\n"\
"6IoXUCisXAfBo3aqttDWY/v1aY+gGDukSbl0QQfU4g==\n"\
"-----END CERTIFICATE-----\n";

#endif
