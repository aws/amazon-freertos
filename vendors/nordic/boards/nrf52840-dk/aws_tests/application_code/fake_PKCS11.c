/*
 * Amazon FreeRTOS
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
 * @file fake_PKCS11.c
 * @brief This port is not using pkcs11, however it is used in the test to retrive code signer.
 */

 /* FreeRTOS includes. */
#include "FreeRTOS.h"

#include "iot_pkcs11.h"

CK_FUNCTION_LIST xFunctionList;

static CK_RV prvC_Initialize(CK_VOID_PTR   pInitArgs)
{
    return CKR_OK;
}

static CK_RV prvC_GetSlotList(  CK_BBOOL       tokenPresent,  /* only slots with tokens? */
                                CK_SLOT_ID_PTR pSlotList,     /* receives array of slot IDs */
                                CK_ULONG_PTR   pulCount )
{
    return CKR_OK;
}

static CK_RV prvC_OpenSession(  CK_SLOT_ID            slotID,        /* the slot's ID */
                                CK_FLAGS              flags,         /* from CK_SESSION_INFO */
                                CK_VOID_PTR           pApplication,  /* passed to callback */
                                CK_NOTIFY             Notify,        /* callback function */
                                CK_SESSION_HANDLE_PTR phSession      /* gets session handle */)
{
    return CKR_OK;
}

static CK_RV prvC_CloseSession( CK_SESSION_HANDLE hSession )
{
    return CKR_OK;
}

CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                                    uint8_t * pucCodeSignCertificate,
                                    size_t xCertificateLength,
                                    uint8_t * pucLabel,
                                    CK_OBJECT_HANDLE_PTR xObjectHandle )
{
    return CKR_OK;
}

CK_RV C_GetFunctionList( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{
  *ppxFunctionList = &xFunctionList;
  xFunctionList.C_Initialize = prvC_Initialize;
  xFunctionList.C_GetSlotList = prvC_GetSlotList; 
  xFunctionList.C_OpenSession = prvC_OpenSession;
  xFunctionList.C_CloseSession = prvC_CloseSession;
  return CKR_OK;
}

