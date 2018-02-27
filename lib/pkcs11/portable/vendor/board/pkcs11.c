/*
 * Amazon FreeRTOS PKCS#11 V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file pkcs11.c
 * @brief PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "aws_pkcs11.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL,
    C_GetFunctionList,
    C_GetSlotList,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_OpenSession,
    C_CloseSession,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_CreateObject,
    NULL,
    C_DestroyObject,
    NULL,
    C_GetAttributeValue,
    NULL,
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_SignInit,
    C_Sign,
    NULL,
    NULL,
    NULL,
    NULL,
    C_VerifyInit,
    C_Verify,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_GenerateRandom,
    NULL,
    NULL,
    NULL
};

/**
 * @brief Hardware polling needed for third-party provided mbedTLS, if you are using 
 * mbedTLS as the underlying TLS support. See 
 * lib\third_party\mbedtls\include\mbedtls\entropy_poll.h for more information.
 */
int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );

/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{
    *ppxFunctionList = &prvP11FunctionList;

    return CKR_OK;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                            CK_SLOT_ID_PTR pxSlotList,
                                            CK_ULONG_PTR pulCount )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID xSlotID,
                                            CK_FLAGS xFlags,
                                            CK_VOID_PTR pvApplication,
                                            CK_NOTIFY xNotify,
                                            CK_SESSION_HANDLE_PTR pxSession )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue )( CK_SESSION_HANDLE xSession,
                                                  CK_OBJECT_HANDLE xObject,
                                                  CK_ATTRIBUTE_PTR pxTemplate,
                                                  CK_ULONG ulCount )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
                                            CK_OBJECT_HANDLE_PTR pxObject,
                                            CK_ULONG ulMaxObjectCount,
                                            CK_ULONG_PTR pulObjectCount )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_Sign )( CK_SESSION_HANDLE xSession,
                                     CK_BYTE_PTR pucData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pucSignature,
                                     CK_ULONG_PTR pulSignatureLen )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_Verify )( CK_SESSION_HANDLE xSession,
                                       CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataLen,
                                       CK_BYTE_PTR pucSignature,
                                       CK_ULONG ulSignatureLen )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{
    /* FIX ME. */
    return CKR_GENERAL_ERROR;
}
/*-----------------------------------------------------------*/

int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
  /* FIX ME. */
  return 0;
}