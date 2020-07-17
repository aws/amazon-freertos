/*
 * FreeRTOS PKCS #11 V1.1.0
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

/* C runtime includes. */
#include <stdlib.h>
#include <stdbool.h>

/* PKCS #11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"

/* Test includes. */
#include "unity.h"

/* Mock includes. */
#include "mock_pkcs11.h"
#include "mock_portable.h"

/* ============================  GLOBAL VARIABLES =========================== */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL, /*C_GetInfo */
    C_GetFunctionList,
    C_GetSlotList,
    C_GetSlotInfo,
    C_GetTokenInfo,
    NULL, /*C_GetMechanismList*/
    C_GetMechanismInfo,
    C_InitToken,
    NULL, /*C_InitPIN*/
    NULL, /*C_SetPIN*/
    C_OpenSession,
    C_CloseSession,
    NULL,    /*C_CloseAllSessions*/
    NULL,    /*C_GetSessionInfo*/
    NULL,    /*C_GetOperationState*/
    NULL,    /*C_SetOperationState*/
    C_Login, /*C_Login*/
    NULL,    /*C_Logout*/
    C_CreateObject,
    NULL,    /*C_CopyObject*/
    C_DestroyObject,
    NULL,    /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL,    /*C_SetAttributeValue*/
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL, /*C_EncryptInit*/
    NULL, /*C_Encrypt*/
    NULL, /*C_EncryptUpdate */
    NULL, /*C_EncryptFinal*/
    NULL, /*C_DecryptInit*/
    NULL, /*C_Decrypt*/
    NULL, /*C_DecryptUpdate*/
    NULL, /*C_DecryptFinal*/
    C_DigestInit,
    NULL, /*C_Digest*/
    C_DigestUpdate,
    NULL, /* C_DigestKey*/
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    NULL, /*C_SignUpdate*/
    NULL, /*C_SignFinal*/
    NULL, /*C_SignRecoverInit*/
    NULL, /*C_SignRecover*/
    C_VerifyInit,
    C_Verify,
    NULL, /*C_VerifyUpdate*/
    NULL, /*C_VerifyFinal*/
    NULL, /*C_VerifyRecoverInit*/
    NULL, /*C_VerifyRecover*/
    NULL, /*C_DigestEncryptUpdate*/
    NULL, /*C_DecryptDigestUpdate*/
    NULL, /*C_SignEncryptUpdate*/
    NULL, /*C_DecryptVerifyUpdate*/
    NULL, /*C_GenerateKey*/
    C_GenerateKeyPair,
    NULL, /*C_WrapKey*/
    NULL, /*C_UnwrapKey*/
    NULL, /*C_DeriveKey*/
    NULL, /*C_SeedRandom*/
    C_GenerateRandom,
    NULL, /*C_GetFunctionStatus*/
    NULL, /*C_CancelFunction*/
    NULL  /*C_WaitForSlotEvent*/
};

static uint16_t usMallocFreeCalls = 0;

/* ==========================  CALLBACK FUNCTIONS =========================== */

/*!
 * @brief Wrapper stub for malloc.
 */
void * pvPkcs11MallocCb( size_t size,
                         int numCalls )
{
    usMallocFreeCalls++;
    return ( void * ) malloc( size );
}

/*!
 * @brief Wrapper stub for malloc that fails every two calls.
 *
 * This is specifically to get complete branch coverage, as some branches
 * are only taken if a previous call to malloc was successful.
 */
void * pvPkcs11MallocCbFailEveryOtherCall( size_t size,
                                           int numCalls )
{
    static uint32_t ulCalls = 1;
    void * pvReturn = NULL;

    ulCalls++;

    if( !( ulCalls % 2 ) )
    {
        usMallocFreeCalls++;
        pvReturn = ( void * ) malloc( size );
    }

    return pvReturn;
}

/*!
 * @brief Wrapper stub for free.
 *
 */
void vPkcs11FreeCb( void * ptr,
                    int numCalls )
{
    usMallocFreeCalls--;
    free( ptr );
}

/*!
 * @brief Create a stub for the PKCS #11 function list.
 *
 */
static CK_RV prvSetFunctionList( CK_FUNCTION_LIST_PTR_PTR ppxPtr )
{
    *ppxPtr = &prvP11FunctionList;
    return CKR_OK;
}

/*!
 * @brief Create a stub for the PKCS #11 function list.
 *
 * Fails on the fourth call in order to create coverage for a nested branch.
 *
 */
static CK_RV prvSetFunctionList2( CK_FUNCTION_LIST_PTR_PTR ppxPtr )
{
    static uint32_t ulCalls = 0;
    CK_RV xResult = CKR_OK;

    ulCalls++;

    /* This case is specifically for the scenario in which prvOpenSession
     * receives a failure when accessing C_GetFunctionList, which would be
     * the 4th call to C_GetFunctionList in the call stack. */
    if( ulCalls == 4 )
    {
        xResult = CKR_ARGUMENTS_BAD;
        *ppxPtr = NULL;
    }
    else
    {
        *ppxPtr = &prvP11FunctionList;
    }

    return xResult;
}

/*!
 * @brief Stub for receiving an uninitialized token.
 *
 */
static CK_RV prvUninitializedToken( CK_SLOT_ID slotID,
                                    CK_TOKEN_INFO_PTR pInfo )
{
    pInfo->flags = CKF_TOKEN_INITIALIZED;
    return CKR_OK;
}

/*!
 * @brief Stub for failing every second call for three argument PKCS #11
 *
 * For example this is used to fail every second call to C_GetSlotList.
 */
static CK_RV xSecondGetFails( CK_BBOOL arg1,
                              CK_SLOT_ID_PTR arg2,
                              CK_ULONG_PTR arg3 )
{
    static uint32_t ulCalls = 1;
    CK_RV xResult = CKR_OK;

    ulCalls++;

    if( ulCalls % 2 )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    return xResult;
}

/*!
 * @brief Sets third pointer arg to 1.
 *
 * Specifically to set the slot count count to be more than 0.
 */
static CK_RV xGet1Item( CK_BBOOL arg1,
                        CK_SLOT_ID_PTR arg2,
                        CK_ULONG_PTR arg3 )
{
    *arg3 = 1;
    return CKR_OK;
}

/*!
 * @brief Sets fourth pointer arg to 1.
 *
 * Specifically to set the object count to be more than 0.
 */
static CK_RV xGet1Item2( CK_BBOOL arg1,
                         CK_SLOT_ID_PTR arg2,
                         CK_ULONG_PTR arg3,
                         CK_ULONG_PTR arg4 )
{
    *arg4 = 1;
    return CKR_OK;
}


/*!
 * @brief Common stubs to most iot_pkcs11.c #11 test paths.
 *
 */
static void vCommonStubs( void )
{
    C_GetFunctionList_IgnoreAndReturn( CKR_OK );
    C_GetFunctionList_Stub( ( void * ) &prvSetFunctionList );

    C_Initialize_IgnoreAndReturn( CKR_OK );
}

/*******************************************************************************
 * Unity fixtures
 ******************************************************************************/
void setUp( void )
{
}

/* called before each testcase */
void tearDown( void )
{
    TEST_ASSERT_EQUAL_INT_MESSAGE( 0, usMallocFreeCalls,
                                   "free is not called the same number of times as malloc, \
        you might have a memory leak!!" );
    usMallocFreeCalls = 0;
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/*!
 * @brief xInitializePKCS11 happy path.
 *
 */
void test_IotPkcs11_xInitializePKCS11( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    xResult = xInitializePKCS11();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief xInitializePKCS11 failed to get function list.
 *
 */
void test_IotPkcs11_xInitializePkcs11BadFunctionList( void )
{
    CK_RV xResult = CKR_OK;

    C_GetFunctionList_IgnoreAndReturn( CKR_ARGUMENTS_BAD );
    xResult = xInitializePKCS11();

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xGetSlotList happy path.
 *
 */
void test_IotPkcs11_xGetSlotList( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID_PTR pxSlotId = NULL;
    CK_ULONG xSlotCount = 0;

    vCommonStubs();
    C_GetSlotList_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = xGetSlotList( &pxSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vPkcs11FreeCb( pxSlotId, 1 );
}

/*!
 * @brief xGetSlotList failed to get function list.
 *
 */
void test_IotPkcs11_xGetSlotListBadFunctionList( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID_PTR pxSlotId = NULL;
    CK_ULONG xSlotCount = 0;

    C_GetFunctionList_IgnoreAndReturn( CKR_ARGUMENTS_BAD );
    pvPortMalloc_IgnoreAndReturn( NULL );
    xResult = xGetSlotList( &pxSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xGetSlotList failed to get slot list.
 *
 */
void test_IotPkcs11_xGetSlotListBadSlotList( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID_PTR pxSlotId = NULL;
    CK_ULONG xSlotCount = 0;

    vCommonStubs();
    C_GetSlotList_IgnoreAndReturn( CKR_ARGUMENTS_BAD );
    pvPortMalloc_IgnoreAndReturn( NULL );
    xResult = xGetSlotList( &pxSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xGetSlotList free memory branch path.
 *
 * This behavior is acquired by failing the second malloc, and forcing xGetSlotList
 * to clean up after itself.
 */
void test_IotPkcs11_xGetSlotListFreeMemory( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID_PTR pxSlotId = NULL;
    CK_ULONG xSlotCount = 0;

    vCommonStubs();
    C_GetSlotList_Stub( ( void * ) xSecondGetFails );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = xGetSlotList( &pxSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xGetSlotList no memory branch path.
 *
 */
void test_IotPkcs11_xGetSlotListNoMemory( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID_PTR pxSlotId = NULL;
    CK_ULONG xSlotCount = 0;

    vCommonStubs();
    C_GetSlotList_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_IgnoreAndReturn( NULL );
    xResult = xGetSlotList( &pxSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );
}

/*!
 * @brief xInitializePkcs11Token happy path.
 *
 */
void test_IotPkcs11_xInitializePkcs11Token( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    C_GetTokenInfo_IgnoreAndReturn( CKR_OK );
    C_InitToken_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief xInitializePkcs11Token C_GetTokenInfo failure due to memory constraint.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenBadC_GetTokenInfo( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    C_GetTokenInfo_IgnoreAndReturn( CKR_HOST_MEMORY );
    C_InitToken_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );
}

/*!
 * @brief xInitializePkcs11Token C_GetTokenInfo failure due to uninitialized token.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenUninitializedToken( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    C_GetTokenInfo_Stub( ( void * ) prvUninitializedToken );
    C_InitToken_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief xInitializePkcs11Token failure due to out of memory.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenMallocFail( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCbFailEveryOtherCall );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    C_GetTokenInfo_IgnoreAndReturn( CKR_OK );
    C_InitToken_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );
}

/*!
 * @brief xInitializePkcs11Token failure due to bad C_GetFunctionList.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenBadFunctionList( void )
{
    CK_RV xResult = CKR_OK;

    C_GetFunctionList_IgnoreAndReturn( CKR_ARGUMENTS_BAD );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xInitializePkcs11Token branch path for a NULL C_GetTokenInfo function.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenNullTokenInfo( void )
{
    CK_RV xResult = CKR_OK;

    prvP11FunctionList.C_GetTokenInfo = NULL;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvP11FunctionList.C_GetTokenInfo = C_GetTokenInfo;
}

/*!
 * @brief xInitializePkcs11Token branch path for a NULL C_InitToken function.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenNullInitToken( void )
{
    CK_RV xResult = CKR_OK;

    prvP11FunctionList.C_InitToken = NULL;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvP11FunctionList.C_InitToken = C_InitToken;
}

/*!
 * @brief xInitializePkcs11Token Happy path in which C_GetSlotList returns greater
 * than 0 tokens.
 *
 */
void test_IotPkcs11_xInitializePkcs11TokenHasTokenInfo( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    C_GetTokenInfo_IgnoreAndReturn( CKR_OK );
    C_InitToken_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Token();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief vAppendSHA256AlgorithmIdentifierSequence Happy path.
 *
 */
void test_IotPkcs11_vAppendSHA256AlgorithmIdentifierSequence( void )
{
    CK_RV xResult = CKR_OK;
    uint8_t pucMessage[] = pkcs11STUFF_APPENDED_TO_RSA_SIG;
    uint8_t pucOidBuf[ 128 ] = { 0 };

    xResult = vAppendSHA256AlgorithmIdentifierSequence( pucMessage, pucOidBuf );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief vAppendSHA256AlgorithmIdentifierSequence Bad input paths.
 *
 */
void test_IotPkcs11_vAppendSHA256AlgorithmIdentifierSequenceBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    uint8_t pucMessage[] = pkcs11STUFF_APPENDED_TO_RSA_SIG;
    uint8_t pucOidBuf[ 128 ] = { 0 };

    xResult = vAppendSHA256AlgorithmIdentifierSequence( NULL, NULL );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = vAppendSHA256AlgorithmIdentifierSequence( pucMessage, NULL );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = vAppendSHA256AlgorithmIdentifierSequence( NULL, pucOidBuf );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xInitializePkcs11Session Happy path.
 *
 */
void test_IotPkcs11_xInitializePkcs11Session( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };

    vCommonStubs();
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_OpenSession_IgnoreAndReturn( CKR_OK );
    C_Login_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Session( &xHandle );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief xInitializePkcs11Session C_Login is a NULL function path.
 *
 */
void test_IotPkcs11_xInitializePkcs11SessionNullC_Login( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };

    prvP11FunctionList.C_Login = NULL;

    vCommonStubs();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    C_OpenSession_IgnoreAndReturn( CKR_OK );
    C_Login_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Session( &xHandle );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvP11FunctionList.C_Login = C_Login;
}

/*!
 * @brief xInitializePkcs11Session NULL args failure.
 *
 */
void test_IotPkcs11_xInitializePkcs11SessionBadArgs( void )
{
    CK_RV xResult = CKR_OK;

    vCommonStubs();
    xResult = xInitializePkcs11Session( NULL );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xInitializePkcs11Session C_GetFunctionList failure path.
 *
 */
void test_IotPkcs11_xInitializePkcs11SessionBadFunctionList( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };

    C_GetFunctionList_IgnoreAndReturn( CKR_ARGUMENTS_BAD );
    xResult = xInitializePkcs11Session( &xHandle );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xInitializePkcs11Session C_GetFunctionList failure path.
 *
 * Fails on the second call to C_GetFunctionList.
 */
void test_IotPkcs11_xInitializePkcs11SessionBadFunctionList2( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };

    C_GetFunctionList_Stub( ( void * ) &prvSetFunctionList2 );
    C_Initialize_IgnoreAndReturn( CKR_OK );
    C_GetSlotList_Stub( ( void * ) xGet1Item );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = xInitializePkcs11Session( &xHandle );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xInitializePkcs11Session C_Initialize returns already initialized.
 *
 */
void test_IotPkcs11_xInitializePkcs11SessionAlreadyInitialized( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };

    C_GetFunctionList_IgnoreAndReturn( CKR_OK );
    C_GetFunctionList_Stub( ( void * ) &prvSetFunctionList );
    C_Initialize_IgnoreAndReturn( CKR_CRYPTOKI_ALREADY_INITIALIZED );
    C_GetSlotList_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    C_OpenSession_IgnoreAndReturn( CKR_OK );
    C_Login_IgnoreAndReturn( CKR_OK );
    xResult = xInitializePkcs11Session( &xHandle );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief xFindObjectWithLabelAndClass Happy path.
 *
 */
void test_IotPkcs11_xFindObjectWithLabelAndClass( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };
    CK_OBJECT_HANDLE xPrivateKeyHandle = { 0 };

    vCommonStubs();
    C_FindObjectsInit_IgnoreAndReturn( CKR_OK );
    C_FindObjects_Stub( ( void * ) xGet1Item2 );
    C_FindObjectsFinal_IgnoreAndReturn( CKR_OK );
    xResult = xFindObjectWithLabelAndClass( xHandle,
                                            pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                            CKO_PRIVATE_KEY, &xPrivateKeyHandle );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief xFindObjectWithLabelAndClass All calls succeed but no object was found.
 *
 */
void test_IotPkcs11_xFindObjectWithLabelAndClassNoObjectsFound( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };
    CK_OBJECT_HANDLE xPrivateKeyHandle = { 0 };

    vCommonStubs();
    C_FindObjectsInit_IgnoreAndReturn( CKR_OK );
    C_FindObjects_IgnoreAndReturn( CKR_OK );
    C_FindObjectsFinal_IgnoreAndReturn( CKR_OK );
    xResult = xFindObjectWithLabelAndClass( xHandle,
                                            pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                            CKO_PRIVATE_KEY, &xPrivateKeyHandle );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( CK_INVALID_HANDLE, xPrivateKeyHandle );
}

/*!
 * @brief xFindObjectWithLabelAndClass Bad arguments test path.
 *
 */
void test_IotPkcs11_xFindObjectWithLabelAndClassNullArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };
    CK_OBJECT_HANDLE xPrivateKeyHandle = { 0 };

    /* NULL label name. */
    xResult = xFindObjectWithLabelAndClass( xHandle, NULL, CKO_PRIVATE_KEY, &xPrivateKeyHandle );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    /* NULL object handle. */
    xResult = xFindObjectWithLabelAndClass( xHandle, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, CKO_PRIVATE_KEY, NULL );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief xFindObjectWithLabelAndClass C_GetFunctionList failure test path.
 *
 */
void test_IotPkcs11_xFindObjectWithLabelAndClassBadFunctionList( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xHandle = { 0 };
    CK_OBJECT_HANDLE xPrivateKeyHandle = { 0 };

    C_GetFunctionList_IgnoreAndReturn( CKR_ARGUMENTS_BAD );
    xResult = xFindObjectWithLabelAndClass( xHandle, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, CKO_PRIVATE_KEY, &xPrivateKeyHandle );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}
