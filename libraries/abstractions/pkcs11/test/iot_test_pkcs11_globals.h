/*
 * Amazon FreeRTOS PKCS#11 V1.0.8
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

#include "unity_fixture.h"
#include "unity.h"
#include "iot_pkcs11.h"

/* Test buffer size definitions. */
#define SHA256_DIGEST_SIZE      32
#define ECDSA_SIGNATURE_SIZE    64
#define RSA_SIGNATURE_SIZE      256

/* PKCS #11 Globals.
 * These are used to reduce setup and tear down calls, and to
 * prevent memory leaks in the case of TEST_PROTECT() actions being triggered. */
CK_SESSION_HANDLE xGlobalSession;
CK_FUNCTION_LIST_PTR pxGlobalFunctionList;
CK_SLOT_ID xGlobalSlotId;
CK_MECHANISM_TYPE xMechanismType;
CK_OBJECT_HANDLE xPublicKey;
CK_OBJECT_HANDLE xPrivateKey;
CK_OBJECT_HANDLE xKey;
CK_BBOOL xCkTrue;
CK_BBOOL xCkFalse;

/* PKCS #11 Global Data Containers. */
CK_BYTE rsaSignature[RSA_SIGNATURE_SIZE];
CK_BYTE rsaHashedMessage[SHA256_DIGEST_SIZE];
CK_BYTE ecdsaHashedMessage[SHA256_DIGEST_SIZE];
CK_BYTE rsaHashPlusOid[pkcs11RSA_SIGNATURE_INPUT_LENGTH];