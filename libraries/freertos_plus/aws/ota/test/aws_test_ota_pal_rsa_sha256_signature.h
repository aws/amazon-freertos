/*
 * FreeRTOS OTA V1.2.1
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

#include <stdint.h>

/**
 * @brief Invalid signature for OTA PAL testing.
 */
static const uint8_t ucInvalidSignature[] =
{
    0x65, 0x2d, 0x7f, 0x38, 0x7d, 0xa7, 0x2b, 0x63, 0x83, 0x01, 0x7d, 0xc8,
    0x5a, 0x49, 0xd8, 0x9e, 0x46, 0xaa, 0x61, 0x32, 0x9b, 0xd6, 0x96, 0xfc,
    0xc3, 0x2a, 0x06, 0xaf, 0xc4, 0x41, 0xf5, 0xe1, 0x5b, 0x61, 0xc7, 0x39,
    0xf6, 0xe8, 0xe3, 0x00, 0xfc, 0xc4, 0x01, 0x63, 0x26, 0x90, 0x5c, 0x1f,
    0x44, 0x1f, 0x43, 0x02, 0xaf, 0x96, 0xd4, 0x3e, 0x88, 0x9e, 0xb6, 0x37,
    0xd1, 0xdd, 0x31, 0x1b, 0xfd, 0x1a, 0x3f, 0xbd, 0x7d, 0x93, 0x8d, 0x69,
    0xbb, 0x3e, 0xfc, 0x44, 0x61, 0xef, 0xc4, 0xce, 0xba, 0xd6, 0x29, 0xb4,
    0x41, 0xc9, 0xab, 0xac, 0xc5, 0x4d, 0x86, 0x0b, 0xd4, 0xb1, 0x26, 0xc3,
    0xa3, 0x9e, 0xea, 0x2b, 0x6c, 0xfa, 0x3f, 0x24, 0x27, 0x86, 0x17, 0x5d,
    0xc0, 0xa6, 0x98, 0xd4, 0x7d, 0x18, 0x47, 0xde, 0xd4, 0xd7, 0x46, 0x95,
    0x60, 0x91, 0x5e, 0xc9, 0xed, 0x87, 0x98, 0x68, 0xb5, 0xe7, 0x83, 0x0c,
    0xaa, 0xfd, 0x36, 0x27, 0x17, 0xfb, 0xb1, 0xb2, 0xed, 0x73, 0x3b, 0xeb,
    0x55, 0xa3, 0x29, 0xb4, 0x80, 0x38, 0xdb, 0x36, 0x3b, 0x9e, 0x67, 0xfd,
    0x33, 0x8b, 0x9b, 0x91, 0xc0, 0xc3, 0x14, 0xb7, 0xbe, 0x97, 0x80, 0xef,
    0x67, 0x97, 0xdf, 0x81, 0x0d, 0x40, 0x38, 0xd9, 0xaa, 0x82, 0xb8, 0x20,
    0x63, 0x91, 0x06, 0xe5, 0x03, 0xa0, 0x65, 0x21, 0x3d, 0xc4, 0x3a, 0xd1,
    0xb5, 0x4d, 0x3d, 0x85, 0x54, 0x5d, 0xfe, 0x0f, 0x0b, 0x42, 0xf4, 0x8a,
    0xfa, 0x69, 0x42, 0xa8, 0x9d, 0x19, 0x45, 0x38, 0x0b, 0x88, 0xdf, 0x28,
    0xd3, 0xd6, 0x57, 0xb5, 0x2f, 0x74, 0x92, 0x2a, 0xb4, 0x2d, 0x0b, 0xa6,
    0xed, 0x1a, 0x6f, 0x13, 0x8f, 0xb8, 0x59, 0x52, 0xc5, 0x31, 0xaf, 0xef,
    0xbc, 0x58, 0x93, 0xbf, 0xf5, 0x8c, 0x4e, 0xc6, 0x54, 0x49, 0x95, 0xca,
    0x9d, 0xdd, 0xd9, 0x5f
};
static const int ucInvalidSignatureLength = 256;

/**
 * @brief Valid signature matching the test block in the OTA PAL tests.
 */
static const uint8_t ucValidSignature[] =
{
    0xa9, 0x5f, 0xa1, 0x5d, 0x9b, 0x42, 0x09, 0x78, 0xaf, 0x95, 0xe4, 0x42,
    0xac, 0xf4, 0xd7, 0x83, 0xea, 0xa9, 0x67, 0x54, 0x32, 0x62, 0x54, 0x21,
    0x46, 0xf5, 0x91, 0xe7, 0x76, 0xdf, 0x2f, 0xa0, 0x6a, 0xe3, 0xb1, 0xd2,
    0xda, 0x1b, 0x6d, 0x54, 0x6b, 0xac, 0xb6, 0x38, 0x1d, 0x3c, 0xf1, 0xcf,
    0x0d, 0x33, 0x84, 0xac, 0x5d, 0xf1, 0x20, 0x7b, 0xfe, 0x2f, 0x9d, 0xf9,
    0x93, 0x5e, 0x4d, 0x34, 0xdc, 0x21, 0x1c, 0x1b, 0x90, 0xf8, 0x37, 0xc6,
    0x7d, 0x16, 0x16, 0x25, 0xe3, 0xcc, 0x06, 0xb5, 0xd5, 0xbf, 0x64, 0x64,
    0x44, 0xa0, 0x60, 0xfe, 0xf3, 0xba, 0xf2, 0xdc, 0xc7, 0x57, 0xbe, 0xca,
    0xcc, 0x6b, 0x57, 0xc2, 0x89, 0x7c, 0xef, 0xdc, 0xc8, 0xf8, 0xb3, 0x34,
    0x8f, 0x67, 0x7a, 0x04, 0x3e, 0x9f, 0x96, 0x17, 0x7f, 0x98, 0x6c, 0x87,
    0x21, 0x70, 0x9d, 0xf8, 0xf4, 0x26, 0xe4, 0xc8, 0x61, 0x57, 0x2a, 0xea,
    0x08, 0xe1, 0x51, 0x52, 0xbb, 0x53, 0x11, 0x35, 0x1e, 0x39, 0x1a, 0x11,
    0x2f, 0xc4, 0xa6, 0x19, 0x5e, 0x6a, 0x62, 0xa1, 0x77, 0x7f, 0x6b, 0xc4,
    0x0a, 0x61, 0xf8, 0x9a, 0x95, 0xd2, 0xfa, 0x09, 0x38, 0xd1, 0xb1, 0xfb,
    0x75, 0xd1, 0x36, 0xfe, 0x2f, 0xc5, 0x08, 0xc1, 0x35, 0x66, 0x7a, 0x97,
    0x46, 0x61, 0x8a, 0xfa, 0xbb, 0x83, 0x63, 0x8c, 0xe9, 0x0b, 0x2a, 0x3e,
    0x88, 0xaa, 0x79, 0x63, 0xb6, 0x92, 0x22, 0xb1, 0x75, 0xeb, 0x20, 0x0d,
    0x06, 0xb2, 0x01, 0x35, 0x01, 0xa2, 0xa7, 0x21, 0x3f, 0xe9, 0x80, 0x83,
    0x87, 0xfd, 0x29, 0x54, 0x77, 0x3a, 0x13, 0xa6, 0x10, 0xa7, 0x47, 0xed,
    0x9f, 0xb1, 0x52, 0xb6, 0x86, 0x54, 0xbc, 0x89, 0x40, 0xa8, 0x74, 0x15,
    0xd8, 0xa7, 0x69, 0x9b, 0x27, 0x7a, 0x99, 0xde, 0x9d, 0xdc, 0x69, 0xb6,
    0x0c, 0x57, 0x68, 0x3d
};
static const int ucValidSignatureLength = 256;

/**
 * @brief The type of signature method this file defines for the valid signature.
 */
#define otatestSIG_METHOD    otatestSIG_SHA256_RSA
