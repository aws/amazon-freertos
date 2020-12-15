/*
 * FreeRTOS PKCS #11 V2.2.0
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
#include "core_pkcs11.h"

extern CK_SESSION_HANDLE xGlobalSession;
extern CK_FUNCTION_LIST_PTR pxGlobalFunctionList;
extern CK_SLOT_ID xGlobalSlotId;
extern CK_MECHANISM_TYPE xGlobalMechanismType;
extern CK_OBJECT_HANDLE xGlobalPublicKeyHandle;
extern CK_OBJECT_HANDLE xGlobalPrivateKeyHandle;
extern CK_OBJECT_HANDLE xKey;
extern CK_BBOOL xGlobalCkTrue;
extern CK_BBOOL xGlobalCkFalse;

CK_BYTE rsaHashPlusOid[ pkcs11RSA_SIGNATURE_INPUT_LENGTH ];
CK_BYTE rsaHashedMessage[ pkcs11SHA256_DIGEST_LENGTH ];
CK_BYTE ecdsaSignature[ pkcs11RSA_2048_SIGNATURE_LENGTH ];
CK_BYTE ecdsaHashedMessage[ pkcs11SHA256_DIGEST_LENGTH ];

CK_RV prvBeforeRunningTests();
void prvAfterRunningTests_NoObject();
void prvAfterRunningTests_Object();

void prvProvisionCredentialsWithKeyImport( CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                           CK_OBJECT_HANDLE_PTR pxCertificateHandle,
                                           CK_OBJECT_HANDLE_PTR pxPublicKeyHandle );

void prvProvisionRsaTestCredentials( CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                     CK_OBJECT_HANDLE_PTR pxPublicKeyHandle,
                                     CK_OBJECT_HANDLE_PTR pxCertificateHandle );

void resetCredentials();
void generateValidSingingKeyPair();

void C_Initialize_normal_behavior();
void C_Initialize_exceptional_behavior_0();

void C_Finalize_normal_behavior();
void C_Finalize_exceptional_behavior_0();
void C_Finalize_exceptional_behavior_1();

void C_CloseSession_normal_behavior();
void C_CloseSession_exceptional_behavior_0();
void C_CloseSession_exceptional_behavior_1();

void C_OpenSession_normal_behavior();
void C_OpenSession_exceptional_behavior_0();
void C_OpenSession_exceptional_behavior_1();
void C_OpenSession_exceptional_behavior_2();

void C_FindObjectsInit_normal_behavior();
void C_FindObjectsInit_exceptional_behavior_0();
void C_FindObjectsInit_exceptional_behavior_1();
void C_FindObjectsInit_exceptional_behavior_2();
void C_FindObjectsInit_exceptional_behavior_3();

void C_FindObjects_normal_behavior();
void C_FindObjects_exceptional_behavior_0();
void C_FindObjects_exceptional_behavior_1();
void C_FindObjects_exceptional_behavior_2();
void C_FindObjects_exceptional_behavior_3();

void C_FindObjectsFinal_normal_behavior();
void C_FindObjectsFinal_exceptional_behavior_0();
void C_FindObjectsFinal_exceptional_behavior_1();
void C_FindObjectsFinal_exceptional_behavior_2();

void C_SignInit_normal_behavior();
void C_SignInit_exceptional_behavior_0();
void C_SignInit_exceptional_behavior_1();
void C_SignInit_exceptional_behavior_2();
void C_SignInit_exceptional_behavior_3();
void C_SignInit_exceptional_behavior_4();
void C_SignInit_exceptional_behavior_5();
void C_SignInit_exceptional_behavior_6();
void C_SignInit_exceptional_behavior_7();
void C_SignInit_exceptional_behavior_8();
void C_SignInit_exceptional_behavior_9();

void C_Sign_normal_behavior();
void C_Sign_exceptional_behavior_0();
void C_Sign_exceptional_behavior_1();
void C_Sign_exceptional_behavior_2();
void C_Sign_exceptional_behavior_3();
void C_Sign_exceptional_behavior_4();

void C_VerifyInit_normal_behavior();
void C_VerifyInit_exceptional_behavior_0();
void C_VerifyInit_exceptional_behavior_1();
void C_VerifyInit_exceptional_behavior_2();
void C_VerifyInit_exceptional_behavior_3();
void C_VerifyInit_exceptional_behavior_4();
void C_VerifyInit_exceptional_behavior_5();
void C_VerifyInit_exceptional_behavior_6();
void C_VerifyInit_exceptional_behavior_7();
void C_VerifyInit_exceptional_behavior_8();

void C_Verify_normal_behavior();
void C_Verify_exceptional_behavior_0();
void C_Verify_exceptional_behavior_1();
void C_Verify_exceptional_behavior_2();
void C_Verify_exceptional_behavior_3();
void C_Verify_exceptional_behavior_4();
void C_Verify_exceptional_behavior_5();
void C_Verify_exceptional_behavior_6();

void C_DigestInit_normal_behavior();
void C_DigestInit_exceptional_behavior_0();
void C_DigestInit_exceptional_behavior_1();
void C_DigestInit_exceptional_behavior_2();
void C_DigestInit_exceptional_behavior_3();
void C_DigestInit_exceptional_behavior_4();

void C_DigestUpdate_normal_behavior();
void C_DigestUpdate_exceptional_behavior_0();
void C_DigestUpdate_exceptional_behavior_1();
void C_DigestUpdate_exceptional_behavior_2();
void C_DigestUpdate_exceptional_behavior_3();

void C_DigestFinal_normal_behavior();
void C_DigestFinal_exceptional_behavior_0();
void C_DigestFinal_exceptional_behavior_1();
void C_DigestFinal_exceptional_behavior_2();
void C_DigestFinal_exceptional_behavior_3();
void C_DigestFinal_exceptional_behavior_4();

void C_GetAttributeValue_normal_behavior();
void C_GetAttributeValue_exceptional_behavior_0();
void C_GetAttributeValue_exceptional_behavior_1();
void C_GetAttributeValue_exceptional_behavior_2();
void C_GetAttributeValue_exceptional_behavior_3();
void C_GetAttributeValue_exceptional_behavior_4();
void C_GetAttributeValue_exceptional_behavior_5();

void C_GenerateKeyPair_normal_behavior();
void C_GenerateKeyPair_exceptional_behavior_0();
void C_GenerateKeyPair_exceptional_behavior_1();
void C_GenerateKeyPair_exceptional_behavior_2();
void C_GenerateKeyPair_exceptional_behavior_3();
void C_GenerateKeyPair_exceptional_behavior_4();
void C_GenerateKeyPair_exceptional_behavior_5();

void C_GenerateRandom_normal_behavior();
void C_GenerateRandom_exceptional_behavior_0();
void C_GenerateRandom_exceptional_behavior_1();
void C_GenerateRandom_exceptional_behavior_2();
