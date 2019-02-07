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
 * "-----END CERTIFICATE-----";\
 *
-----BEGIN CERTIFICATE-----
MIIBbzCCARWgAwIBAgIJAJPFr0x06QwnMAoGCCqGSM49BAMCMCoxKDAmBgNVBAMM
H210YWxyZWphK2N1c3RvbWVyb3RhQGFtYXpvbi5jb20wHhcNMTgxMTE2MTUxMzE1
WhcNMTkxMTE2MTUxMzE1WjAqMSgwJgYDVQQDDB9tdGFscmVqYStjdXN0b21lcm90
YUBhbWF6b24uY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEX9UHRtAZaJYf
BUcqKmU9rafpT8gL/59yDops6KpgEKAV2oM+0RoouJrW3+9BRjgR5vrR726yOQkQ
DWihXqjnY6MkMCIwCwYDVR0PBAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMAoG
CCqGSM49BAMCA0gAMEUCIQDC+nr5D9RYg5sJEIkUsb3Q75wFbayZ3K4CPWIfcYJ0
vAIgPJKkhIw4glamLLCt2uDCeVTd7ci69HYBbe55Ai7DsiE=
-----END CERTIFICATE-----
 *
 */
static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
		"-----BEGIN CERTIFICATE-----\n"\
		"MIIBVTCB/aADAgECAgkAkyxmLG7cLWEwCgYIKoZIzj0EAwIwHjEcMBoGA1UEAwwT\n"\
		"YmhhZ2F2YXJAYW1hem9uLmNvbTAeFw0xODExMjgwMDQzMjFaFw0xOTExMjgwMDQz\n"\
		"MjFaMB4xHDAaBgNVBAMME2JoYWdhdmFyQGFtYXpvbi5jb20wWTATBgcqhkjOPQIB\n"\
		"BggqhkjOPQMBBwNCAATvBxOkYnVGY3VIvPnaschtE6U1wDYPnMkR0d+HZAAwsteG\n"\
		"XS0GXeMt05W+nKjIQXQ2PbIDE/82Hg3sNkMOfklSoyQwIjALBgNVHQ8EBAMCB4Aw\n"\
		"EwYDVR0lBAwwCgYIKwYBBQUHAwMwCgYIKoZIzj0EAwIDRwAwRAIgFh0q8ThmwSi3\n"\
		"Yigp9IK0WvO8KnLNDsGwU11U0mbBkCQCIF76X6yqLEGCo8gvVn4ZVWGmHeSImmMk\n"\
		"R4P1nP5uKSqv\n"\
		"-----END CERTIFICATE-----\n";


#endif
