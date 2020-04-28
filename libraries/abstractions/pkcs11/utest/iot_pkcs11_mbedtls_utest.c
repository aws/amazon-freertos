/*
 * FreeRTOS PKCS #11 V2.0.3
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Mock includes. */
#include "mock_iot_crypto.h"
#include "mock_queue.h"
#include "mock_ctr_drbg.h"
#include "mock_entropy.h"

/* PKCS #11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"

/* Test includes. */
#include "unity.h"



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
void * pvPkcs11MallocCb2( size_t size,
                          int numCalls )
{
    static uint32_t ulCalls = 1;

    ulCalls++;

    if( ulCalls % 2 )
    {
        return NULL;
    }

    usMallocFreeCalls++;
    return ( void * ) malloc( size );
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
 * @brief C_Initialize happy path.
 *
 */
void test_pkcs11_C_Initialize( void )
{
    CK_RV xResult = CKR_OK;

    /* Hack: Giving out pointer to xResult when creating a mutex. This unit testcase
     * will never use the actual mutex. */
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( NULL );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_Initialize memory failure when creating a mutex.
 *
 */
void test_pkcs11_C_InitializeMemFail( void )
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( NULL );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );
}

/*!
 * @brief C_Initialize failed to seed DRBG.
 *
 */
void test_pkcs11_C_InitializeSeedFail( void )
{
    CK_RV xResult = CKR_OK;

    /* Hack: Giving out pointer to xResult when creating a mutex. This unit testcase
     * will never use the actual mutex. */
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 1 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );
}

/*!
 * @brief C_Initialize already initialized.
 *
 */
void test_pkcs11_C_InitializeInitTwice( void )
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_Initialize( NULL );
    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_ALREADY_INITIALIZED, xResult );

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( NULL );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}


/*!
 * @brief C_Finalize happy path.
 *
 */
void test_pkcs11_C_Finalize( void )
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( NULL );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_Finalize initialize was not already called.
 *
 */
void test_pkcs11_C_FinalizeUninitialized( void )
{
    CK_RV xResult = CKR_OK;

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( NULL );

    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_NOT_INITIALIZED, xResult );
}

/*!
 * @brief C_Finalize was called with a non-null pointer.
 *
 */
void test_pkcs11_C_FinalizeBadArgs( void )
{
    CK_RV xResult = CKR_OK;

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( ( CK_VOID_PTR ) &xResult );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief C_GetFunctionList happy path.
 *
 */
void test_pkcs11_C_GetFunctionList( void )
{
    CK_RV xResult = CKR_OK;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;

    xResult = C_GetFunctionList( &pxFunctionList );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetFunctionList bad args.
 *
 */
void test_pkcs11_C_GetFunctionListBadArgs( void )
{
    CK_RV xResult = CKR_OK;

    xResult = C_GetFunctionList( NULL );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}
